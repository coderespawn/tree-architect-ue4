//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "Tree.h"

//using namespace respawn::renderer;
//using namespace respawn::math;
//using namespace respawn;
//using namespace minitree;
//using namespace stl;
//
//int BranchNode::_idCounter = 0;
//
//
//void minitree::SpaceNode::UpdateClosestNode( BranchNodePtr node )
//{
//	if (closestNode != node) {
//		Point3 nodePosition = node->GetPosition();
//		Vector3 delta = this->position - nodePosition;
//		float distance2 = lengthSqr(delta);
//		if (distance2 < distanceSqToClosestNode) {
//			closestNode = node;
//			distanceSqToClosestNode = distance2;
//		}
//	}
//}
//
//minitree::SpaceNode::SpaceNode( const respawn::math::Point3& position, BranchNodePtr closestNode ) 
//	: closestNode(closestNode), distanceSqToClosestNode(static_cast<float>(INT_MAX))
//{
//	this->position = position;
//}
//
//minitree::BranchNode::BranchNode( BranchNodePtr parent, TreeDataPtr tree, const respawn::math::Point3& position ) 
//		: parent(parent), tree(tree), thicknessUnits(0), vertexIndexBase(0), distanceFromRoot(0)
//{
//	id = ++_idCounter;
//	this->position = position;
//	attractionVector = Vector3(0, 0, 0);
//}
//
//minitree::BranchNodePtr minitree::BranchNode::Grow(float outerDiameter)
//{
//	if (attractionVector == Vector3(0, 0, 0)) {
//		// This node is not being influenced. do no grow on this node
//		return BranchNodePtr();
//	}
//
//	attractionVector = normalize(attractionVector);
//	Vector3 offset = attractionVector * outerDiameter;
//	return AddChild(position + offset);
//}
//
//minitree::BranchNodePtr minitree::BranchNode::AddChild( const respawn::math::Point3& childPosition )
//{
//	BranchNodePtr child = BranchNodePtr(new BranchNode(shared_from_this(), tree, childPosition));
//	children.push_back(child);
//	return child;
//}
//
//void minitree::BranchNode::InitializeForNextGrowth()
//{
//	// attractionVector = new Vector3.zero();
//	attractionVector = Vector3(0, 0, 0);
//}
//
//void minitree::BranchNode::Dispose()
//{
//	parent.reset();
//	tree.reset();
//	//children.clear();
//	vertices.clear();
//}
//
//
//minitree::TreeData::TreeData()
//{
//}
//
//void minitree::TreeData::Init()
//{
//	root = BranchNodePtr(new BranchNode(BranchNodePtr(), shared_from_this(), respawn::math::Point3(0, 0, 0)));
//}
//
//void minitree::TreeData::Dispose()
//{
//
//	// Destroy all branch nodes in the tree
//	TreeTraverser<TreeData> traverser;
//	traverser.Traverse(shared_from_this(), this, &TreeData::_DisposeBranchNode);
//
//	root.reset();
//	meshData.clear();
//	spacePoints.clear();
//	trunkGeometry.triangles.clear();
//	trunkGeometry.vertices.clear();
//	leavesGeometry.triangles.clear();
//	leavesGeometry.vertices.clear();
//}
//
//void minitree::TreeData::_DisposeBranchNode( BranchNodePtr node )
//{
//	if (node) {
//		node->Dispose();
//	}
//}
//
//
//bool minitree::TriangleData::operator<( const TriangleData& other )
//{
//	return distanceFromCamera > other.distanceFromCamera;
//}
