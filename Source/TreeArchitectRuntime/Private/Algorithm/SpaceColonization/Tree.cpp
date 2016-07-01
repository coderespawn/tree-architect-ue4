//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "Tree.h"

int BranchNode::_idCounter = 0;


void SpaceNode::UpdateClosestNode( BranchNodePtr node )
{
	if (closestNode != node) {
		FVector nodePosition = node->GetPosition();
		FVector delta = this->position - nodePosition;
		float distance2 = delta.SizeSquared();
		if (distance2 < distanceSqToClosestNode) {
			closestNode = node;
			distanceSqToClosestNode = distance2;
		}
	}
}

SpaceNode::SpaceNode( const FVector& position, BranchNodePtr closestNode ) 
	: closestNode(closestNode), distanceSqToClosestNode(static_cast<float>(INT_MAX))
{
	this->position = position;
}

BranchNode::BranchNode( BranchNodePtr parent, TreeDataPtr tree, const FVector& position ) 
		: parent(parent), tree(tree), thicknessUnits(0), vertexIndexBase(0), distanceFromRoot(0)
{
	id = ++_idCounter;
	this->position = position;
	attractionVector = FVector(0, 0, 0);
}

BranchNodePtr BranchNode::Grow(float outerDiameter)
{
	if (attractionVector == FVector(0, 0, 0)) {
		// This node is not being influenced. do no grow on this node
		return BranchNodePtr();
	}

	attractionVector.Normalize();
	FVector offset = attractionVector * outerDiameter;
	return AddChild(position + offset);
}

BranchNodePtr BranchNode::AddChild( const FVector& childPosition )
{
	BranchNodePtr child = BranchNodePtr(new BranchNode(SharedThis(this), tree, childPosition));
	children.Add(child);
	return child;
}

void BranchNode::InitializeForNextGrowth()
{
	// attractionVector = new FVector.zero();
	attractionVector = FVector(0, 0, 0);
}

void BranchNode::Dispose()
{
	parent.Reset();
	tree.Reset();
	vertices.Reset();
}


TreeData::TreeData()
{
}

void TreeData::Init()
{
	root = BranchNodePtr(new BranchNode(BranchNodePtr(), SharedThis(this), FVector(0, 0, 0)));
}

void TreeData::Dispose()
{

	// Destroy all branch nodes in the tree
	TreeTraverser<TreeData> traverser;
	traverser.Traverse(SharedThis(this), this, &TreeData::_DisposeBranchNode);

	root.Reset();
	meshData.Reset();
	spacePoints.Reset();
	trunkGeometry.triangles.Reset();
	trunkGeometry.vertices.Reset();
	leavesGeometry.triangles.Reset();
	leavesGeometry.vertices.Reset();
}

void TreeData::_DisposeBranchNode( BranchNodePtr node )
{
	if (node.IsValid()) {
		node->Dispose();
	}
}


bool TriangleData::operator<( const TriangleData& other )
{
	return distanceFromCamera > other.distanceFromCamera;
}
