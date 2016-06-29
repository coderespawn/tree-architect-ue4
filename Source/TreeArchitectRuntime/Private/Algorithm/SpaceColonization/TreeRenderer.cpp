//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeRenderer.h"
#include "Tree.h"

//#include "eastl/sort.h"
//using namespace respawn::renderer;
//using namespace respawn::math;
//using namespace respawn;
//using namespace minitree;
//using namespace stl;
//
//
//
//minitree::TreeDebugRenderer::TreeDebugRenderer( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//	: tree(tree), materialName("TreeDebug"), dirty(true)
//{
//	mesh = DynamicMeshPtr(new DynamicMesh(BLM_DYNAMIC_WRITE));
//	mesh->CreateVertexBuffer(rendererContext, VBT_POSITION);
//	mesh->CreateVertexBuffer(rendererContext, VBT_COLOR);
//}
//
//const stl::string& minitree::TreeDebugRenderer::GetMaterialName() const
//{
//	return materialName;
//}
//
//respawn::renderer::AxisAlignedBoundingBox minitree::TreeDebugRenderer::GetObjectAABB() const
//{
//	return AxisAlignedBoundingBox();
//}
//
//void minitree::TreeDebugRendererSpacePoints::Draw( const RendererContext& rendererContext, ShaderProgram* shader )
//{
//	const SpaceNodeList& nodes = tree->GetSpacePoints();
//	int vertexCount = nodes.size();
//	if (vertexCount == 0) {
//		// Nothing to draw
//		return;
//	}
//
//	// Re-upload the new vertex data to the GPU
//	mesh->Clear();
//	VertexPositionList& positions = mesh->GetPositions();
//	VertexColorList& colors = mesh->GetColors();
//
//	Color pointColor(1, 0, 0, 1);
//	for (SpaceNodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
//		SpaceNodePtr spaceNode = *it;
//		const Point3 spacePoint = spaceNode->GetPosition();
//		positions.push_back(VertexPosition(spacePoint.getX(), spacePoint.getY(), spacePoint.getZ()));
//		colors.push_back(pointColor);
//	}
//
//	mesh->Bind(shader);
//	mesh->UpdateBuffers();
//	rendererContext.GetRenderSystem().SetPointSize(4);
//	rendererContext.GetRenderSystem().DrawPrimitive(ROT_POINTLIST, vertexCount);
//	mesh->UnBind();
//}
//
//minitree::TreeDebugRendererSpacePoints::TreeDebugRendererSpacePoints( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//	: TreeDebugRenderer(rendererContext, tree)
//{
//
//}
//
//
//
//minitree::TreeDebugRendererBranchNodes::TreeDebugRendererBranchNodes( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//	: TreeDebugRenderer(rendererContext, tree)
//{
//	lineMesh = DynamicMeshPtr(new DynamicMesh(BLM_DYNAMIC_WRITE));
//	lineMesh->CreateVertexBuffer(rendererContext, VBT_POSITION);
//	lineMesh->CreateVertexBuffer(rendererContext, VBT_COLOR);
//}
//
//void minitree::TreeDebugRendererBranchNodes::Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader )
//{
//	if (dirty) {
//		dirty = false;
//		UploadGeometry(rendererContext, shader);
//	}
//
//	mesh->Bind(shader);
//	rendererContext.GetRenderSystem().SetPointSize(4);
//	rendererContext.GetRenderSystem().DrawPrimitive(ROT_POINTLIST, mesh->GetPositions().size());
//	mesh->UnBind();
//
//	lineMesh->Bind(shader);
//	rendererContext.GetRenderSystem().DrawPrimitive(ROT_LINELIST, lineMesh->GetPositions().size());
//	lineMesh->UnBind();
//}
//
//void minitree::TreeDebugRendererBranchNodes::UploadGeometry( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader )
//{
//	lineMesh->Clear();
//	mesh->Clear();
//
//	TreeTraverser<TreeDebugRendererBranchNodes> traverser;
//	traverser.Traverse(tree, this, &TreeDebugRendererBranchNodes::VisitNode);
//
//	// Draw the points
//	if (mesh->GetPositions().size() > 0) {
//		mesh->Bind(shader);
//		mesh->UpdateBuffers();
//		mesh->UnBind();
//	}
//
//	// Draw the skeleton lines
//	if (lineMesh->GetPositions().size() > 0) {
//		lineMesh->Bind(shader);
//		lineMesh->UpdateBuffers();
//		lineMesh->UnBind();
//	}
//}
//
//void minitree::TreeDebugRendererBranchNodes::VisitNode( BranchNodePtr node )
//{
//	const Point3 position = node->GetPosition();
//	VertexPosition startPoint(position.getX(), position.getY(), position.getZ());
//	mesh->GetPositions().push_back(startPoint);
//	mesh->GetColors().push_back(VertexColor(0, 0, 1, 1));
//
//	if (node->GetParent()) {
//		VertexColor lineColor(1, 1, 0, 1);
//		const Point3 parentPosition = node->GetParent()->GetPosition();
//		VertexPosition endPoint(parentPosition.getX(), parentPosition.getY(), parentPosition.getZ());
//		lineMesh->GetPositions().push_back(startPoint);
//		lineMesh->GetPositions().push_back(endPoint);
//
//		lineMesh->GetColors().push_back(lineColor);
//		lineMesh->GetColors().push_back(lineColor);
//	}
//}
//
//minitree::TreeDebugRendererWireframe::TreeDebugRendererWireframe( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//	: TreeDebugRenderer(rendererContext, tree), indexSize(0)
//{
//}
//
//void minitree::TreeDebugRendererWireframe::Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader )
//{
//	if (dirty) {
//		dirty = false;
//		UploadGeometry(rendererContext, shader);
//	}
//
//	if (mesh->GetPositions().size() == 0) return;
//
//	mesh->Bind(shader);
//	indexBuffer->Bind();
//	rendererContext.GetRenderSystem().DrawIndexedPrimitive(ROT_LINELIST, indexSize, sizeof(short));
//	indexBuffer->UnBind();
//	mesh->UnBind();
//}
//
//void minitree::TreeDebugRendererWireframe::UploadGeometry( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader )
//{
//	mesh->Clear();
//	const LineInfoList& lines = tree->GetMeshData();
//	const TreeVertexDataList& vertices = tree->GetTrunkGeometry().vertices;
//	VertexPositionList& positions = mesh->GetPositions();
//	VertexColorList& colors = mesh->GetColors();
//
//	Color meshColor(1, 0, 0, 1);
//
//	// Prepare the indices
//	indexSize = lines.size() * 2;
//	short* indices = new short[indexSize];
//	int i = 0;
//	for (LineInfoList::const_iterator it = lines.begin(); it != lines.end(); ++it) {
//		const LineInfo& line = *it;
//		indices[i++] = static_cast<short>(line.startIndex);
//		indices[i++] = static_cast<short>(line.endIndex);
//	}
//
//	int elementSize = sizeof(short);
//	if (!indexBuffer) {
//		indexBuffer = rendererContext.GetRenderSystem().CreateIndexBuffer(BLM_DYNAMIC_WRITE, indexSize, elementSize, elementSize, indices);
//	} else {
//		indexBuffer->Write(indices, 0, indexSize * elementSize);
//	}
//	delete[] indices;
//
//	// Prepare the vertices
//	for (TreeVertexDataList::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
//		const TreeVertexData& vertex = *it;
//		const Point3& point = vertex.position;
//		positions.push_back(VertexPosition(point.getX(), point.getY(), point.getZ()));
//		colors.push_back(meshColor);
//	}
//
//	// upload the points
//	if (mesh->GetPositions().size() > 0) {
//		mesh->Bind(shader);
//		mesh->UpdateBuffers();
//		mesh->UnBind();
//	}
//}
//
//uint32 minitree::TreeDebugRendererWireframe::GetVertexCount() const
//{
//	return mesh->GetPositions().size();
//}
//
//minitree::TreeRenderer::TreeRenderer( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//	: tree(tree), dirty(true), indexSize(0), sortIndices(false)
//{
//	mesh = DynamicMeshPtr(new DynamicMesh(BLM_DYNAMIC_WRITE));
//	mesh->CreateVertexBuffer(rendererContext, VBT_POSITION);
//	mesh->CreateVertexBuffer(rendererContext, VBT_NORMAL);
//	mesh->CreateVertexBuffer(rendererContext, VBT_TEXCOORD0);
//}
//
//void minitree::TreeRenderer::Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader )
//{
//	UploadGeometry(rendererContext, shader);
//
//	if (mesh->GetPositions().size() == 0) return;
//
//	mesh->Bind(shader);
//	indexBuffer->Bind();
//	rendererContext.GetRenderSystem().DrawIndexedPrimitive(ROT_TRIANGLELIST, indexSize, sizeof(uint16));
//	indexBuffer->UnBind();
//	mesh->UnBind();
//
//}
//
//uint32 minitree::TreeRenderer::GetVertexCount() const
//{
//	return mesh->GetPositions().size();
//}
//
//void minitree::TreeRenderer::UploadGeometry( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader )
//{
//	if (dirty) {
//		mesh->Clear();
//
//		const TreeVertexDataList& vertices = GetGeometry().vertices;
//
//		VertexPositionList& meshPositions = mesh->GetPositions();
//		VertexNormalList& meshNormals = mesh->GetNormals();
//		VertexTexCoordList& texcoords = mesh->GetTexCoods0();
//
//		// Prepare the vertices
//		Color meshColor(1, 1, 0, 1);
//		for (TreeVertexDataList::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
//			const TreeVertexData& vertex = *it;
//			const Point3& point = vertex.position;
//			const Vector3& normal = vertex.normal;
//			const Vector2& uv = vertex.uv;
//			meshPositions.push_back(VertexPosition(point.getX(), point.getY(), point.getZ()));
//			meshNormals.push_back(VertexNormal(normal.getX(), normal.getY(), normal.getZ()));
//			texcoords.push_back(VertexTexCoord(uv.x, uv.y));
//		}
//
//		// upload the vertices
//		if (mesh->GetPositions().size() > 0) {
//			mesh->Bind(shader);
//			mesh->UpdateBuffers();
//			mesh->UnBind();
//		}
//	}
//
//	if (sortIndices || dirty) {
//		// Prepare the indices
//		TriangleDataList& triangles = GetGeometry().triangles;
//		indexSize = triangles.size() * 3;
//		int elementSize = sizeof(uint16);
//
//		if (sortIndices) {
//			//sort the triangles based on the distance from the camera
//			Point3 camPosition = rendererContext.GetRenderSystem().GetCamera().GetPosition();
//			for (TriangleDataList::iterator it = triangles.begin(); it != triangles.end(); ++it) {
//				TriangleData& triangle = *it;
//				triangle.distanceFromCamera = lengthSqr(triangle.center - camPosition);
//			}
//
//			int triangleCount = triangles.size();
//			std::sort(triangles.begin(), triangles.end());
//		}
//		if (frameIndices.size() < indexSize) {
//			frameIndices.resize(indexSize);
//		}
//		for (size_t i = 0; i < triangles.size(); i++) {
//			frameIndices[i * 3 + 0] = triangles[i].v0;
//			frameIndices[i * 3 + 1] = triangles[i].v1;
//			frameIndices[i * 3 + 2] = triangles[i].v2;
//		}
//		if (!indexBuffer) {
//			indexBuffer = rendererContext.GetRenderSystem().CreateIndexBuffer(BLM_DYNAMIC_WRITE, indexSize, elementSize, elementSize, &frameIndices[0]);
//		} else {
//			indexBuffer->Write(&frameIndices[0], 0, indexSize * elementSize);
//		}
//	}
//	dirty = false;
//}
//
//const stl::string& minitree::TreeRenderer::GetMaterialName() const
//{
//	return materialName;
//}
//
//respawn::renderer::AxisAlignedBoundingBox minitree::TreeRenderer::GetObjectAABB() const
//{
//	return AxisAlignedBoundingBox();
//}
//
//minitree::TreeTrunkRenderer::TreeTrunkRenderer( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//		: TreeRenderer(rendererContext, tree)
//{
//	materialName = "TreeTrunkBasic";
//}
//
//TreeGeometry& minitree::TreeTrunkRenderer::GetGeometry()
//{
//	return tree->GetTrunkGeometry();
//}
//
//minitree::TreeLeafRenderer::TreeLeafRenderer( const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree )
//	: TreeRenderer(rendererContext, tree)
//{
//	materialName = "TreeLeafBasic";
//	sortIndices = true;
//}
//
//TreeGeometry& minitree::TreeLeafRenderer::GetGeometry()
//{
//	return tree->GetLeavesGeometry();
//}
