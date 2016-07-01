// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved. 
//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
// Modified version of UProceduralMeshComponent

#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeMeshComponent.h"
#include "DynamicMeshBuilder.h"
#include "PhysicsEngine/BodySetup.h"
#include "Algorithm/SpaceColonization/Tree.h"

DECLARE_CYCLE_STAT(TEXT("Tree Create ProcMesh Proxy"), STAT_TreeProcMesh_CreateSceneProxy, STATGROUP_TreeMesh);
DECLARE_CYCLE_STAT(TEXT("Tree Create Mesh Section"), STAT_TreeProcMesh_CreateMeshSection, STATGROUP_TreeMesh);
DECLARE_CYCLE_STAT(TEXT("Tree UpdateSection GT"), STAT_TreeProcMesh_UpdateSectionGT, STATGROUP_TreeMesh);
DECLARE_CYCLE_STAT(TEXT("Tree UpdateSection RT"), STAT_TreeProcMesh_UpdateSectionRT, STATGROUP_TreeMesh);
DECLARE_CYCLE_STAT(TEXT("Tree Get ProcMesh Elements"), STAT_TreeProcMesh_GetMeshElements, STATGROUP_TreeMesh);
DECLARE_CYCLE_STAT(TEXT("Tree Update Collision"), STAT_TreeProcMesh_UpdateCollision, STATGROUP_TreeMesh);
DEFINE_LOG_CATEGORY(LogTreeMeshComponent);

/** Resource array to pass  */
class FTreeProcMeshVertexResourceArray : public FResourceArrayInterface
{
public:
	FTreeProcMeshVertexResourceArray(void* InData, uint32 InSize)
		: Data(InData)
		, Size(InSize)
	{
	}

	virtual const void* GetResourceData() const override { return Data; }
	virtual uint32 GetResourceDataSize() const override { return Size; }
	virtual void Discard() override { }
	virtual bool IsStatic() const override { return false; }
	virtual bool GetAllowCPUAccess() const override { return false; }
	virtual void SetAllowCPUAccess(bool bInNeedsCPUAccess) override { }

private:
	void* Data;
	uint32 Size;
};

/** Vertex Buffer */
class FTreeProcMeshVertexBuffer : public FVertexBuffer
{
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() override
	{
		const uint32 SizeInBytes = Vertices.Num() * sizeof(FDynamicMeshVertex);

		FTreeProcMeshVertexResourceArray ResourceArray(Vertices.GetData(), SizeInBytes);
		FRHIResourceCreateInfo CreateInfo(&ResourceArray);
		VertexBufferRHI = RHICreateVertexBuffer(SizeInBytes, BUF_Static, CreateInfo);
	}

};

/** Index Buffer */
class FTreeProcMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		void* Buffer = nullptr;
		IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo, Buffer);

		// Write the indices to the index buffer.		
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FTreeProcMeshVertexFactory : public FLocalVertexFactory
{
public:

	FTreeProcMeshVertexFactory()
	{}

	/** Init function that should only be called on render thread. */
	void Init_RenderThread(const FTreeProcMeshVertexBuffer* VertexBuffer)
	{
		check(IsInRenderingThread());

		// Initialize the vertex factory's stream components.
		FDataType NewData;
		NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
		NewData.TextureCoordinates.Add(
			FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
			);
		NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
		NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
		NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
		SetData(NewData);
	}

	/** Init function that can be called on any thread, and will do the right thing (enqueue command if called on main thread) */
	void Init(const FTreeProcMeshVertexBuffer* VertexBuffer)
	{
		if (IsInRenderingThread())
		{
			Init_RenderThread(VertexBuffer);
		}
		else
		{
			ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
				InitProcMeshVertexFactory,
				FTreeProcMeshVertexFactory*, VertexFactory, this,
				const FTreeProcMeshVertexBuffer*, VertexBuffer, VertexBuffer,
				{
				VertexFactory->Init_RenderThread(VertexBuffer);
			});
		}
	}
};

/** Class representing a single section of the proc mesh */
class FTreeProcMeshProxySection
{
public:
	/** Material applied to this section */
	UMaterialInterface* Material;
	/** Vertex buffer for this section */
	FTreeProcMeshVertexBuffer VertexBuffer;
	/** Index buffer for this section */
	FTreeProcMeshIndexBuffer IndexBuffer;
	/** Vertex factory for this section */
	FTreeProcMeshVertexFactory VertexFactory;
	/** Whether this section is currently visible */
	bool bSectionVisible;

	FTreeProcMeshProxySection()
	: Material(NULL)
	, bSectionVisible(true)
	{}
};

struct FTreeLODProxyResource {

	/** Array of sections */
	TArray<FTreeProcMeshProxySection*> Sections;

	FTreeLODModelInfo LODInfo;
};

/** 
 *	Struct used to send update to mesh data 
 *	Arrays may be empty, in which case no update is performed.
 */
class FTreeProcMeshSectionUpdateData
{
public:
	/** Section to update */
	int32 TargetSection;
	/** New vertex information */
	TArray<FTreeProcMeshVertex> NewVertexBuffer;
};

static void LAConvertProcMeshToDynMeshVertex(FDynamicMeshVertex& Vert, const FTreeProcMeshVertex& ProcVert)
{
	Vert.Position = ProcVert.Position;
	Vert.Color = ProcVert.Color;
	Vert.TextureCoordinate = ProcVert.UV0;
	Vert.TangentX = ProcVert.Tangent.TangentX;
	Vert.TangentZ = ProcVert.Normal;
	Vert.TangentZ.Vector.W = ProcVert.Tangent.bFlipTangentY ? 0 : 255;
}

/** Procedural mesh scene proxy */
class FTreeProceduralMeshSceneProxy : public FPrimitiveSceneProxy
{
public:

	FTreeProceduralMeshSceneProxy(UTreeMeshComponent* Component)
		: FPrimitiveSceneProxy(Component)
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		// Copy each section
		for (FTreeLODModel& LOD : Component->LODModels) {
			TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;
			const int32 NumSections = ProcMeshSections.Num();
			LODResources.AddDefaulted();
			FTreeLODProxyResource& LODResource = LODResources[LODResources.Num() - 1];
			LODResource.LODInfo = LOD.LODInfo;

			TArray<FTreeProcMeshProxySection*>& Sections = LODResource.Sections;
			Sections.AddZeroed(NumSections);
			for (int SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
			{
				FTreeProcMeshSection& SrcSection = ProcMeshSections[SectionIdx];
				if (SrcSection.ProcIndexBuffer.Num() > 0 && SrcSection.ProcVertexBuffer.Num() > 0)
				{
					FTreeProcMeshProxySection* NewSection = new FTreeProcMeshProxySection();

					// Copy data from vertex buffer
					const int32 NumVerts = SrcSection.ProcVertexBuffer.Num();

					// Allocate verts
					NewSection->VertexBuffer.Vertices.SetNumUninitialized(NumVerts);
					// Copy verts
					for (int VertIdx = 0; VertIdx < NumVerts; VertIdx++)
					{
						const FTreeProcMeshVertex& ProcVert = SrcSection.ProcVertexBuffer[VertIdx];
						FDynamicMeshVertex& Vert = NewSection->VertexBuffer.Vertices[VertIdx];
						LAConvertProcMeshToDynMeshVertex(Vert, ProcVert);
					}

					// Copy index buffer
					NewSection->IndexBuffer.Indices = SrcSection.ProcIndexBuffer;

					// Init vertex factory
					NewSection->VertexFactory.Init(&NewSection->VertexBuffer);

					// Enqueue initialization of render resource
					BeginInitResource(&NewSection->VertexBuffer);
					BeginInitResource(&NewSection->IndexBuffer);
					BeginInitResource(&NewSection->VertexFactory);

					// Grab material
					NewSection->Material = LOD.LODInfo.Material; //Component->GetMaterial(SectionIdx);
					if (NewSection->Material == NULL)
					{
						NewSection->Material = UMaterial::GetDefaultMaterial(MD_Surface);
					}

					// Copy visibility info
					NewSection->bSectionVisible = SrcSection.bSectionVisible;

					// Save ref to new section
					Sections[SectionIdx] = NewSection;
				}
			}
		}
	}

	virtual ~FTreeProceduralMeshSceneProxy()
	{
		for (FTreeLODProxyResource& LODResource : LODResources) {
			TArray<FTreeProcMeshProxySection*>& Sections = LODResource.Sections;

			for (FTreeProcMeshProxySection* Section : Sections)
			{
				if (Section != nullptr)
				{
					Section->VertexBuffer.ReleaseResource();
					Section->IndexBuffer.ReleaseResource();
					Section->VertexFactory.ReleaseResource();
					delete Section;
				}
			}
		}
	}

	/*
	// Called on render thread to assign new dynamic data
	void UpdateSection_RenderThread(FTreeProcMeshSectionUpdateData* SectionData)
	{
		SCOPE_CYCLE_COUNTER(STAT_TreeProcMesh_UpdateSectionRT);

		check(IsInRenderingThread());

		// Check we have data 
		if(	SectionData != nullptr) 			
		{
			// Check it references a valid section
			if (SectionData->TargetSection < Sections.Num() &&
				Sections[SectionData->TargetSection] != nullptr)
			{
				FTreeProcMeshProxySection* Section = Sections[SectionData->TargetSection];

				// Lock vertex buffer
				const int32 NumVerts = SectionData->NewVertexBuffer.Num();
				FDynamicMeshVertex* VertexBufferData = (FDynamicMeshVertex*)RHILockVertexBuffer(Section->VertexBuffer.VertexBufferRHI, 0, NumVerts * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
			
				// Iterate through vertex data, copying in new info
				for(int32 VertIdx=0; VertIdx<NumVerts; VertIdx++)
				{
					const FTreeProcMeshVertex& ProcVert = SectionData->NewVertexBuffer[VertIdx];
					FDynamicMeshVertex& Vert = VertexBufferData[VertIdx];
					LAConvertProcMeshToDynMeshVertex(Vert, ProcVert);
				}

				// Unlock vertex buffer
				RHIUnlockVertexBuffer(Section->VertexBuffer.VertexBufferRHI);
			}

			// Free data sent from game thread
			delete SectionData;
		}
	}
	*/

	void SetSectionVisibility_RenderThread(int32 SectionIndex, bool bNewVisibility)
	{
		check(IsInRenderingThread());

		for (FTreeLODProxyResource& LODResource : LODResources) {
			TArray<FTreeProcMeshProxySection*>& Sections = LODResource.Sections;

			if (SectionIndex < Sections.Num() &&
				Sections[SectionIndex] != nullptr)
			{
				Sections[SectionIndex]->bSectionVisible = bNewVisibility;
			}
		}
	}

	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) override {
		bool bWireframe = false;

		for (const FTreeLODProxyResource& LODResource : LODResources) {
			const TArray<FTreeProcMeshProxySection*>& Sections = LODResource.Sections;

			// Iterate over sections
			for (const FTreeProcMeshProxySection* Section : Sections)
			{
				if (Section != nullptr && Section->bSectionVisible)
				{
					FMaterialRenderProxy* MaterialProxy = Section->Material ? Section->Material->GetRenderProxy(IsSelected()) : nullptr;

					// Draw the mesh.
					FMeshBatch Mesh;
					FMeshBatchElement& BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = &Section->IndexBuffer;
					Mesh.bWireframe = bWireframe;
					Mesh.VertexFactory = &Section->VertexFactory;
					Mesh.MaterialRenderProxy = MaterialProxy;
					BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
					BatchElement.FirstIndex = 0;
					BatchElement.NumPrimitives = Section->IndexBuffer.Indices.Num() / 3;
					BatchElement.MinVertexIndex = 0;
					BatchElement.MaxVertexIndex = Section->VertexBuffer.Vertices.Num() - 1;
					Mesh.bDitheredLODTransition = true;
					Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
					Mesh.Type = PT_TriangleList;
					Mesh.DepthPriorityGroup = SDPG_World;
					Mesh.bCanApplyViewModeOverrides = false;

					// Set LOD information
					Mesh.LODIndex = LODResource.LODInfo.LODIndex;
					Mesh.bDitheredLODTransition = true;
					BatchElement.MinScreenSize = LODResource.LODInfo.MinScreenSize;
					BatchElement.MaxScreenSize = LODResource.LODInfo.MaxScreenSize;

					PDI->DrawMesh(Mesh, BatchElement.MaxScreenSize);
				}
			}
		}
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		//SCOPE_CYCLE_COUNTER(STAT_TreeProcMesh_GetMeshElements);

		// Set up wireframe material (if needed)
		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;
		if (bWireframe)
		{
			WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
				FLinearColor(0, 0.5f, 1.f)
				);

			Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
		}


		if (bWireframe) {
			for (const FTreeLODProxyResource& LODResource : LODResources) {
				const TArray<FTreeProcMeshProxySection*>& Sections = LODResource.Sections;

				// Iterate over sections
				for (const FTreeProcMeshProxySection* Section : Sections)
				{
					if (Section != nullptr && Section->bSectionVisible)
					{
						FMaterialRenderProxy* MaterialProxy = bWireframe ? WireframeMaterialInstance : Section->Material->GetRenderProxy(IsSelected());

						// For each view..
						for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
						{
							if (VisibilityMap & (1 << ViewIndex))
							{
								const FSceneView* View = Views[ViewIndex];
								// Draw the mesh.
								FMeshBatch& Mesh = Collector.AllocateMesh();
								FMeshBatchElement& BatchElement = Mesh.Elements[0];
								BatchElement.IndexBuffer = &Section->IndexBuffer;
								Mesh.bWireframe = bWireframe;
								Mesh.VertexFactory = &Section->VertexFactory;
								Mesh.MaterialRenderProxy = MaterialProxy;
								BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
								BatchElement.FirstIndex = 0;
								BatchElement.NumPrimitives = Section->IndexBuffer.Indices.Num() / 3;
								BatchElement.MinVertexIndex = 0;
								BatchElement.MaxVertexIndex = Section->VertexBuffer.Vertices.Num() - 1;
								Mesh.bDitheredLODTransition = true;
								Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
								Mesh.Type = PT_TriangleList;
								Mesh.DepthPriorityGroup = SDPG_World;
								Mesh.bCanApplyViewModeOverrides = false;

								// Set LOD information
								Mesh.LODIndex = LODResource.LODInfo.LODIndex;
								Mesh.bDitheredLODTransition = true;
								BatchElement.MinScreenSize = LODResource.LODInfo.MinScreenSize;
								BatchElement.MaxScreenSize = LODResource.LODInfo.MaxScreenSize;

								Collector.AddMesh(ViewIndex, Mesh);
							}
						}
					}
				}
			}
		}

		// Draw bounds
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				// Render bounds
				RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
			}
		}
#endif
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		Result.bStaticRelevance = true;
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const
	{
		return(sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return(FPrimitiveSceneProxy::GetAllocatedSize());
	}

private:
	/** Array of sections */
	//TArray<FTreeProcMeshProxySection*> Sections;
	TArray<FTreeLODProxyResource> LODResources;

	FMaterialRelevance MaterialRelevance;
};

//////////////////////////////////////////////////////////////////////////


UTreeMeshComponent::UTreeMeshComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void UTreeMeshComponent::CreateMeshSection(int32 SectionIndex, TArray<TreeDataPtr> TreeDataList, const TArray<FTreeLODModelInfo>& LODList, bool bEnableCollision) {
	if (TreeDataList.Num() != LODList.Num()) {
		UE_LOG(LogTreeMeshComponent, Error, TEXT("Invalid no. of elements in Geometry and LOD list"));
		return;
	}

	LODModels.Empty();
	for (int LODIndex = 0; LODIndex < LODList.Num(); LODIndex++) {
		TreeDataPtr TreeData = TreeDataList[LODIndex];
		const TreeGeometry& TrunkGeometry = TreeData->GetTrunkGeometry();
		
		TArray<FVector> Vertices;
		TArray<FVector> Normals;
		TArray<int32> Triangles;
		TArray<FTreeProcMeshTangent> Tangents;
		TArray<FVector2D> UV0;
		TArray<FColor> VertexColors;

		int32 NumVertices = TrunkGeometry.vertices.Num();
		Vertices.Reset(NumVertices);
		Normals.Reset(NumVertices);
		UV0.Reset(NumVertices);
		Triangles.Reset(TrunkGeometry.triangles.Num());

		for (const TreeVertexData& Vertex : TrunkGeometry.vertices) {
			Vertices.Add(Vertex.position);
			Normals.Add(Vertex.normal);
			UV0.Add(Vertex.uv);
		}

		for (const TriangleData& Triangle : TrunkGeometry.triangles) {
			Triangles.Add(Triangle.v0);
			Triangles.Add(Triangle.v2);
			Triangles.Add(Triangle.v1);
		}

		const FTreeLODModelInfo& LODInfo = LODList[LODIndex];

		LODModels.AddDefaulted();
		FTreeLODModel& LODModel = LODModels[LODModels.Num() - 1];
		LODModel.LODInfo = LODInfo;

		TArray<FTreeProcMeshSection>& ProcMeshSections = LODModel.ProcMeshSections;

		// Ensure sections array is long enough
		ProcMeshSections.SetNum(SectionIndex + 1, false);

		// Reset this section (in case it already existed)
		FTreeProcMeshSection& NewSection = ProcMeshSections[SectionIndex];
		NewSection.Reset();

		// Copy data to vertex buffer
		const int32 NumVerts = Vertices.Num();
		NewSection.ProcVertexBuffer.Reset();
		NewSection.ProcVertexBuffer.AddUninitialized(NumVerts);
		for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
		{
			FTreeProcMeshVertex& Vertex = NewSection.ProcVertexBuffer[VertIdx];

			Vertex.Position = Vertices[VertIdx];
			Vertex.Normal = (Normals.Num() == NumVerts) ? Normals[VertIdx] : FVector(0.f, 0.f, 1.f);
			Vertex.UV0 = (UV0.Num() == NumVerts) ? UV0[VertIdx] : FVector2D(0.f, 0.f);
			Vertex.Color = (VertexColors.Num() == NumVerts) ? VertexColors[VertIdx] : FColor(255, 255, 255);
			Vertex.Tangent = (Tangents.Num() == NumVerts) ? Tangents[VertIdx] : FTreeProcMeshTangent();

			// Update bounding box
			NewSection.SectionLocalBox += Vertex.Position;
		}

		// Copy index buffer (clamping to vertex range)
		int32 NumTriIndices = Triangles.Num();
		NumTriIndices = (NumTriIndices / 3) * 3; // Ensure we have exact number of triangles (array is multiple of 3 long)

		NewSection.ProcIndexBuffer.Reset();
		NewSection.ProcIndexBuffer.AddUninitialized(NumTriIndices);
		for (int32 IndexIdx = 0; IndexIdx < NumTriIndices; IndexIdx++)
		{
			NewSection.ProcIndexBuffer[IndexIdx] = FMath::Min(Triangles[IndexIdx], NumVerts - 1);
		}
		NewSection.bEnableCollision = bEnableCollision;
	}

	UpdateLocalBounds(); // Update overall bounds
	UpdateCollision(); // Mark collision as dirty
	MarkRenderStateDirty(); // New section requires recreating scene proxy
}

void UTreeMeshComponent::UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FTreeProcMeshTangent>& Tangents)
{
	/*
	SCOPE_CYCLE_COUNTER(STAT_TreeProcMesh_UpdateSectionGT);

	if(SectionIndex < ProcMeshSections.Num())
	{
		FTreeProcMeshSection& Section = ProcMeshSections[SectionIndex];
		const int32 NumVerts = Section.ProcVertexBuffer.Num();

		// See if positions are changing
		const bool bPositionsChanging = (Vertices.Num() == NumVerts);

		// Update bounds, if we are getting new position data
		if (bPositionsChanging)
		{
			Section.SectionLocalBox.Init();
		}

		// Iterate through vertex data, copying in new info
		for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
		{
			FTreeProcMeshVertex& ModifyVert = Section.ProcVertexBuffer[VertIdx];

			// Position data
			if (Vertices.Num() == NumVerts)
			{
				ModifyVert.Position = Vertices[VertIdx];
				Section.SectionLocalBox += ModifyVert.Position;
			}

			// Normal data
			if (Normals.Num() == NumVerts)
			{
				ModifyVert.Normal = Normals[VertIdx];
			}

			// Tangent data 
			if (Tangents.Num() == NumVerts)
			{
				ModifyVert.Tangent = Tangents[VertIdx];
			}

			// UV data
			if (UV0.Num() == NumVerts)
			{
				ModifyVert.UV0 = UV0[VertIdx];
			}

			// Color data
			if (VertexColors.Num() == NumVerts)
			{
				ModifyVert.Color = VertexColors[VertIdx];
			}
		}

		// Create data to update section
		FTreeProcMeshSectionUpdateData* SectionData = new FTreeProcMeshSectionUpdateData;
		SectionData->TargetSection = SectionIndex;
		SectionData->NewVertexBuffer = Section.ProcVertexBuffer;

		// Enqueue command to send to render thread
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			FProcMeshSectionUpdate,
			FTreeProceduralMeshSceneProxy*, ProcMeshSceneProxy, (FTreeProceduralMeshSceneProxy*)SceneProxy,
			FTreeProcMeshSectionUpdateData*, SectionData, SectionData,
			{
				ProcMeshSceneProxy->UpdateSection_RenderThread(SectionData);
			}
		);

		// If we have collision enabled on this section, update that too
		if(bPositionsChanging && Section.bEnableCollision)
		{
			TArray<FVector> CollisionPositions;

			// We have one collision mesh for all sections, so need to build array of _all_ positions
			for (const FTreeProcMeshSection& CollisionSection : ProcMeshSections)
			{
				// If section has collision, copy it
				if (CollisionSection.bEnableCollision)
				{
					for (int32 VertIdx = 0; VertIdx < CollisionSection.ProcVertexBuffer.Num(); VertIdx++)
					{
						CollisionPositions.Add(CollisionSection.ProcVertexBuffer[VertIdx].Position);
					}
				}
			}

			// Pass new positions to trimesh
			BodyInstance.UpdateTriMeshVertices(CollisionPositions);
		}

		if (bPositionsChanging)
		{
			UpdateLocalBounds(); // Update overall bounds
			MarkRenderTransformDirty(); // Need to send new bounds to render thread
		}
	}
	*/
}

void UTreeMeshComponent::ClearMeshSection(int32 SectionIndex)
{
	for (FTreeLODModel& LOD : LODModels) {
		TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;
		if (SectionIndex < ProcMeshSections.Num())
		{
			ProcMeshSections[SectionIndex].Reset();
		}
	}
	UpdateLocalBounds();
	UpdateCollision();
	MarkRenderStateDirty();
}

void UTreeMeshComponent::ClearAllMeshSections()
{
	for (FTreeLODModel& LOD : LODModels) {
		TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;
		ProcMeshSections.Empty();
	}

	UpdateLocalBounds();
	UpdateCollision();
	MarkRenderStateDirty();
}

/*
void UTreeMeshComponent::SetMeshSectionVisible(int32 SectionIndex, bool bNewVisibility)
{

	for (FTreeLODModel& LOD : Component->LODModels) {
		TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;
		if (SectionIndex < ProcMeshSections.Num())
		{
			// Set game thread state
			ProcMeshSections[SectionIndex].bSectionVisible = bNewVisibility;

			// Enqueue command to modify render thread info
			ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
				FProcMeshSectionVisibilityUpdate,
				FTreeProceduralMeshSceneProxy*, ProcMeshSceneProxy, (FTreeProceduralMeshSceneProxy*)SceneProxy,
				int32, SectionIndex, SectionIndex,
				bool, bNewVisibility, bNewVisibility,
				{
					ProcMeshSceneProxy->SetSectionVisibility_RenderThread(SectionIndex, bNewVisibility);
				}
			);
		}
	}
}

bool UTreeMeshComponent::IsMeshSectionVisible(int32 SectionIndex) const
{
	return (SectionIndex < ProcMeshSections.Num()) ? ProcMeshSections[SectionIndex].bSectionVisible : false;
}

*/

void UTreeMeshComponent::UpdateLocalBounds()
{
	for (FTreeLODModel& LOD : LODModels) {
		TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;
		FBox LocalBox(0);

		for (const FTreeProcMeshSection& Section : ProcMeshSections)
		{
			LocalBox += Section.SectionLocalBox;
		}

		LocalBounds = LocalBox.IsValid ? FBoxSphereBounds(LocalBox) : FBoxSphereBounds(FVector(0, 0, 0), FVector(0, 0, 0), 0); // fallback to reset box sphere bounds
	}

	// Update global bounds
	UpdateBounds();
}

FPrimitiveSceneProxy* UTreeMeshComponent::CreateSceneProxy()
{
	SCOPE_CYCLE_COUNTER(STAT_TreeProcMesh_CreateSceneProxy);

	return new FTreeProceduralMeshSceneProxy(this);
}

int32 UTreeMeshComponent::GetNumMaterials() const
{
	for (const FTreeLODModel& LOD : LODModels) {
		const TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;
		return ProcMeshSections.Num();
	}
	return 0;
}

FBoxSphereBounds UTreeMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return LocalBounds.TransformBy(LocalToWorld);
}

bool UTreeMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	/*
	for (int i = 0; i < CollisionVertices.Num(); i++) {
		CollisionData->Vertices.Add(CollisionVertices[i]);
	}

	for (int i = 0; i < CollisionIndices.Num(); i += 3) {
		// Need to add base offset for indices
		FTriIndices Triangle;
		Triangle.v0 = CollisionIndices[i + 0];
		Triangle.v1 = CollisionIndices[i + 1];
		Triangle.v2 = CollisionIndices[i + 2];
		CollisionData->Indices.Add(Triangle);

		CollisionData->MaterialIndices.Add(0);
	}
	*/

	int32 VertexBase = 0; // Base vertex index for current section
	for (FTreeLODModel& LOD : LODModels) {
		TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;

		// For each section..
		for (int32 SectionIdx = 0; SectionIdx < ProcMeshSections.Num(); SectionIdx++)
		{
			FTreeProcMeshSection& Section = ProcMeshSections[SectionIdx];
			// Do we have collision enabled?
			if (Section.bEnableCollision)
			{
				// Copy vert data
				for (int32 VertIdx = 0; VertIdx < Section.ProcVertexBuffer.Num(); VertIdx++)
				{
					CollisionData->Vertices.Add(Section.ProcVertexBuffer[VertIdx].Position);
				}

				// Copy triangle data
				const int32 NumTriangles = Section.ProcIndexBuffer.Num() / 3;
				for (int32 TriIdx = 0; TriIdx < NumTriangles; TriIdx++)
				{
					// Need to add base offset for indices
					FTriIndices Triangle;
					Triangle.v0 = Section.ProcIndexBuffer[(TriIdx * 3) + 0] + VertexBase;
					Triangle.v1 = Section.ProcIndexBuffer[(TriIdx * 3) + 1] + VertexBase;
					Triangle.v2 = Section.ProcIndexBuffer[(TriIdx * 3) + 2] + VertexBase;
					CollisionData->Indices.Add(Triangle);

					// Also store matrial info
					CollisionData->MaterialIndices.Add(SectionIdx);
				}

				// Remember the base index that new verts will be added from in next section
				VertexBase = CollisionData->Vertices.Num();
			}
		}
	}

	CollisionData->bFlipNormals = true;

	return true;
}

bool UTreeMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	for (const FTreeLODModel& LOD : LODModels) {
		const TArray<FTreeProcMeshSection>& ProcMeshSections = LOD.ProcMeshSections;

		for (const FTreeProcMeshSection& Section : ProcMeshSections)
		{
			if (Section.ProcIndexBuffer.Num() >= 3 && Section.bEnableCollision)
			{
				return true;
			}
		}
	}

	return false;
}

void UTreeMeshComponent::CreateProcMeshBodySetup()
{
	if (ProcMeshBodySetup == NULL)
	{
		ProcMeshBodySetup = NewObject<UBodySetup>(this);
		ProcMeshBodySetup->BodySetupGuid = FGuid::NewGuid();

		ProcMeshBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ProcMeshBodySetup->bGenerateMirroredCollision = false;
		ProcMeshBodySetup->bDoubleSidedGeometry = true;
	}
}

void UTreeMeshComponent::UpdateCollision()
{
	SCOPE_CYCLE_COUNTER(STAT_TreeProcMesh_UpdateCollision);

	bool bCreatePhysState = false; // Should we create physics state at the end of this function?

	// If its created, shut it down now
	if (bPhysicsStateCreated)
	{
		DestroyPhysicsState();
		bCreatePhysState = true;
	}

	// Ensure we have a BodySetup
	CreateProcMeshBodySetup();

#if WITH_RUNTIME_PHYSICS_COOKING || WITH_EDITOR
	// Clear current mesh data
	ProcMeshBodySetup->InvalidatePhysicsData();
	// Create new mesh data
	ProcMeshBodySetup->CreatePhysicsMeshes();
#endif // WITH_RUNTIME_PHYSICS_COOKING || WITH_EDITOR

	// Create new instance state if desired
	if (bCreatePhysState)
	{
		CreatePhysicsState();
	}
}

UBodySetup* UTreeMeshComponent::GetBodySetup()
{
	CreateProcMeshBodySetup();
	return ProcMeshBodySetup;
}


