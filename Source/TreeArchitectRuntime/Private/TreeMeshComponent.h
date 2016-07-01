// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Components/MeshComponent.h"
#include "TreeMeshComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTreeMeshComponent, Log, All);

class TreeData;
typedef TSharedPtr<TreeData> TreeDataPtr;

USTRUCT()
struct TREEARCHITECTRUNTIME_API FTreeLODInfo {
	GENERATED_USTRUCT_BODY()

	FTreeLODInfo() : LODIndex(0), ScreenSize(1.0f), BlockCellsRatio(1.0f), bEnableCollision(false), Material(nullptr) {}

	UPROPERTY()
	int32 LODIndex;

	UPROPERTY(EditAnywhere, Category = "LOD")
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = "LOD")
	float ScreenSize;

	UPROPERTY(EditAnywhere, Category = "LOD")
	float BlockCellsRatio;

	UPROPERTY(EditAnywhere, Category = "LOD")
	bool bEnableCollision;

};

/**
*	Struct used to specify a tangent vector for a vertex
*	The Y tangent is computed from the cross product of the vertex normal (Tangent Z) and the TangentX member.
*/
USTRUCT(BlueprintType)
struct FTreeProcMeshTangent
{
	GENERATED_USTRUCT_BODY()

	/** Direction of X tangent for this vertex */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
	FVector TangentX;

	/** Bool that indicates whether we should flip the Y tangent when we compute it using cross product */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
	bool bFlipTangentY;

	FTreeProcMeshTangent()
		: TangentX(1.f, 0.f, 0.f)
		, bFlipTangentY(false)
	{}

	FTreeProcMeshTangent(float X, float Y, float Z)
		: TangentX(X, Y, Z)
		, bFlipTangentY(false)
	{}

	FTreeProcMeshTangent(FVector InTangentX, bool bInFlipTangentY)
		: TangentX(InTangentX)
		, bFlipTangentY(bInFlipTangentY)
	{}
};

/** One vertex for the procedural mesh, used for storing data internally */
USTRUCT()
struct FTreeProcMeshVertex
{
	GENERATED_USTRUCT_BODY()

	/** Vertex position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	FVector Position;

	/** Vertex normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	FVector Normal;

	/** Vertex tangent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	FTreeProcMeshTangent Tangent;

	/** Vertex color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	FColor Color;

	/** Vertex texture co-ordinate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	FVector2D UV0;


	FTreeProcMeshVertex()
		: Position(0.f, 0.f, 0.f)
		, Normal(0.f, 0.f, 1.f)
		, Tangent(FVector(1.f, 0.f, 0.f), false)
		, Color(255, 255, 255)
		, UV0(0.f, 0.f)
	{}
};

/** One section of the procedural mesh. Each material has its own section. */
USTRUCT()
struct FTreeProcMeshSection
{
	GENERATED_USTRUCT_BODY()

	/** Vertex buffer for this section */
	UPROPERTY()
	TArray<FTreeProcMeshVertex> ProcVertexBuffer;

	/** Index buffer for this section */
	UPROPERTY()
	TArray<int32> ProcIndexBuffer;
	/** Local bounding box of section */
	UPROPERTY()
	FBox SectionLocalBox;

	/** Should we build collision data for triangles in this section */
	UPROPERTY()
	bool bEnableCollision;

	/** Should we display this section */
	UPROPERTY()
	bool bSectionVisible;

	FTreeProcMeshSection()
		: SectionLocalBox(0)
		, bEnableCollision(false)
		, bSectionVisible(true)
	{}

	/** Reset this section, clear all mesh info. */
	void Reset()
	{
		ProcVertexBuffer.Empty();
		ProcIndexBuffer.Empty();
		SectionLocalBox.Init();
		bEnableCollision = false;
		bSectionVisible = true;
	}
};

USTRUCT()
struct FTreeLODModelInfo {
	GENERATED_USTRUCT_BODY()

	FTreeLODModelInfo() : LODIndex(0), MinScreenSize(0), MaxScreenSize(1.0f), Material(nullptr) {}

	UPROPERTY()
	int32 LODIndex;

	UPROPERTY()
	float MinScreenSize;

	UPROPERTY()
	float MaxScreenSize;

	UPROPERTY()
	UMaterialInterface* Material;

};

USTRUCT()
struct FTreeLODModel
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FTreeProcMeshSection> ProcMeshSections;

	UPROPERTY()
	FTreeLODModelInfo LODInfo;
};

/**
*	Component that allows you to specify custom triangle mesh geometry
*	Beware! This feature is experimental and may be substantially changed in future releases.
*/
UCLASS() //hidecategories = (Object), meta = (BlueprintSpawnableComponent))
class TREEARCHITECTRUNTIME_API UTreeMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_UCLASS_BODY()

	/**
	 *	Create/replace a section for this procedural mesh component.
	 *	@param	SectionIndex		Index of the section to create or replace.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	 */
	void CreateMeshSection(int32 SectionIndex, TArray<TreeDataPtr> TreeDataList, const TArray<FTreeLODModelInfo>& LODList, bool bEnableCollision);

	/**
	 *	Updates a section of this procedural mesh component. This is faster than CreateMeshSection, but does not let you change topology. Collision info is also updated.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 */
	void UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FTreeProcMeshTangent>& Tangents);

	/** Clear a section of the procedural mesh. Other sections do not change index. */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void ClearMeshSection(int32 SectionIndex);

	/** Clear all mesh sections and reset to empty state */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void ClearAllMeshSections();

	///** Control visibility of a particular section */
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	//void SetMeshSectionVisible(int32 SectionIndex, bool bNewVisibility);

	///** Returns whether a particular section is currently visible */
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	//bool IsMeshSectionVisible(int32 SectionIndex) const;

	//~ Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override{ return false; }
	//~ End Interface_CollisionDataProvider Interface

	/** Collision data */
	UPROPERTY(transient, duplicatetransient)
	class UBodySetup* ProcMeshBodySetup;


	/** Array of sections of mesh */
	UPROPERTY()
	TArray<FTreeLODModel> LODModels;

private:

	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin UMeshComponent Interface.
	virtual int32 GetNumMaterials() const override;
	//~ End UMeshComponent Interface.



	/** Update LocalBounds member from the local box of each section */
	void UpdateLocalBounds();
	/** Ensure ProcMeshBodySetup is allocated and configured */
	void CreateProcMeshBodySetup();
	/** Mark collision data as dirty, and re-create on instance if necessary */
	void UpdateCollision();

	/** Local space bounds of mesh */
	UPROPERTY()
	FBoxSphereBounds LocalBounds;
	
	friend class FTreeProceduralMeshSceneProxy;
};


