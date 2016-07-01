//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeGenerator.h"
#include "Tree.h"

FTreeConfig::FTreeConfig() 
	: seed(0)
	, numSpacePoints(1000)
	, minSpacePoints(50)
	, attractionPointRadius(200)
	, heightOffset(350.0f)
	, innerRadius(2.5f)
	, outerRadius(5.0f)
	, killDistance(20.0f)
	, smoothingStrength(0.6f)
	, branchTipArea(5.0f)
	, optimizationMaxTilt(3.0f)
	, smoothMaxTilt(0.30f)
	, circumferenceResolution(12)
{

}

float FTreeConfig::GetOuterDiameter() const
{
	return outerRadius * 2;
}


TreeDataPtr TreeGenerator::Generate( const FTreeConfig& config )
{
	this->config = config;
	TreeDataPtr tree = TreeDataPtr(new TreeData());
	tree->Init();

	// Create space point clouds for growing the tree
	FVector pointOnSphere;
	SpaceNodeList& spacePoints = tree->GetSpacePoints();
	spacePoints.Reset(config.numSpacePoints);
	FRandomStream Random;
	Random.Initialize(config.seed);
	for (int i = 0; i < config.numSpacePoints; i++) {
		pointOnSphere = Random.GetUnitVector();
		float depth = pow(Random.FRand(), 1 / 3.0f);
		pointOnSphere = FVector(FVector(pointOnSphere) * (depth * config.attractionPointRadius));

		/*
		if (pointOnSphere.X < 0) {
			pointOnSphere.Z *= 0.5f;
		}
		if (pointOnSphere.X > 0) {
			pointOnSphere.X *= 1.25f;
		} else {
			pointOnSphere.X *= 0.75f;
		}
		*/

		pointOnSphere.Z += config.heightOffset; //.setY(pointOnSphere.getY() + config.heightOffset);
		SpaceNodePtr spaceNode(new SpaceNode(pointOnSphere, tree->GetRoot()));
		spacePoints.Add(spaceNode);
	}
	tree->SetGenerationState(TreeGenerationState::AttractionPoints);

	return tree;
}

void TreeGenerator::OnBranchNodeAdded( TreeDataPtr tree, BranchNodePtr node )
{
	// Traverse through all the space nodes and check if this node is closer than 
	// the previously registered node 
	//tree.spacePoints.forEach((SpaceNode spaceNode) => spaceNode.updateClosestNode(node));
	SpaceNodeList& spaceNodes = tree->GetSpacePoints();
	//for (SpaceNodeList::iterator it = spaceNodes.begin(); it != spaceNodes.end(); ++it) {
	//	SpaceNodePtr spaceNode = *it;
	for (SpaceNodePtr spaceNode : spaceNodes) {
		spaceNode->UpdateClosestNode(node);
	}
}

void TreeGenerator::Grow( TreeDataPtr tree, int maxFrames /*= 1*/, float maxProcessingTime /*= 16*/ )
{
	int framesElapsed = 0;
	while (true) {
		if (tree->GetGenerationState() != TreeGenerationState::SkeletonGenerationProgress) {
			// Not in the correct state (or generation is done)
			break;
		}

		// Grow all the branch frames in the tree by one step
		GrowBranchFrame(tree);

		framesElapsed++;
		if (maxFrames > 0 && framesElapsed >= maxFrames) {
			// Generated the max requested frames. bail out
			break;
		}

		//if (maxProcessingTime > 0 && timer->GetElapsedTime() >= maxProcessingTime) {
		//	// The max processing time has elapsed.  bail out
		//	break;
		//}
	}
}

TreeGenerator::TreeGenerator() 
		: branchNodeVertexIndexCounter(0)
{
	memset(&config, 0, sizeof(config));
}

void TreeGenerator::_InitializeForNextGrowth( BranchNodePtr node )
{
	node->InitializeForNextGrowth();
}

void TreeGenerator::GrowBranchFrame(TreeDataPtr tree)
{
	if (tree->GetGenerationState() != TreeGenerationState::SkeletonGenerationProgress) {
		// Wrong state
		return;
	}

	// Initialize all the nodes in the tree for next growth frame
	TreeTraverser<TreeGenerator> traverser;
	traverser.Traverse(tree, this, &TreeGenerator::_InitializeForNextGrowth);

	// Find the nodes that will grow in this frame
	typedef TMap<int, BranchNodePtr> BranchNodeMap;
	BranchNodeMap growthNodes;
	SpaceNodeList& spacePoints = tree->GetSpacePoints();
	for (SpaceNodePtr spaceNode : spacePoints) {
		BranchNodePtr branchNode = spaceNode->GetClosestNode();
		if (!growthNodes.Contains(branchNode->GetId())) {
			growthNodes.Add(branchNode->GetId(), branchNode);
		}
		else {
			growthNodes[branchNode->GetId()] = branchNode;
		}
	}

	// Update the attraction vector for all the branch nodes
	FVector attractionVector(0, 0, 0);
	for (SpaceNodePtr spaceNode : spacePoints) {
		BranchNodePtr branchNode = spaceNode->GetClosestNode();
		attractionVector = (spaceNode->GetPosition() - branchNode->GetPosition());
		attractionVector.Normalize();
		branchNode->SetAttractionVector(branchNode->GetAttractionVector() + attractionVector);
	}

	// Grow the tree
	int nodesGrown = 0;
	TArray<BranchNodePtr> growthNodeValues;
	growthNodes.GenerateValueArray(growthNodeValues);
	for (BranchNodePtr node : growthNodeValues) {
		BranchNodePtr newNode = node->Grow(config.GetOuterDiameter());
		if (newNode.IsValid()) {
			nodesGrown++;
			OnBranchNodeAdded(tree, newNode);
		}
	}

	// Remove nearby attraction points
	const float killDistance = config.killDistance;
	const float killDistance2 = killDistance * killDistance;
	const float innerRadius = config.innerRadius;
	const float innerRadius2 = innerRadius * innerRadius;
	
	int index = 0;
	while (index < spacePoints.Num()) {
		SpaceNodePtr spacePoint = spacePoints[index];
		if (spacePoint->GetDistanceSqToClosestNode() - innerRadius2 <= killDistance2) {
			// Point is too close. consume it
			spacePoints.RemoveAt(index);
		}
		else {
			++index;
		}
	}

	// If too few space points are left, the complete the generation process
	if (spacePoints.Num() <= config.minSpacePoints) {
		spacePoints.Reset();
		tree->SetGenerationState(TreeGenerationState::SkeletonGenerated);
	}
}

BranchNodePtr TreeGenerator::DecimateNode( BranchNodePtr node, float maxTiltAngle )
{
	BranchNodeList& children = node->GetChildren();
	if (children.Num() != 1) {
		// Do not remove this node since the branch splits here
		return node;
	}

	if (children.Num() == 0) {
		// Reached a leaf node.  Cannot remove this
		return node;
	}

	BranchNodePtr currentNode = children[0];
	FVector referenceDirection = (node->GetPosition() - currentNode->GetPosition());
	referenceDirection.Normalize();
	while (currentNode->GetChildren().Num() == 1) {
		// Check if the next node is too off from the reference direction
		BranchNodePtr nextNode = currentNode->GetChildren()[0];
		FVector currentDirection = (node->GetPosition() - nextNode->GetPosition());
		currentDirection.Normalize();
		
		// Find the angle between the two vectors
		float dotProduct = FVector::DotProduct(currentDirection, referenceDirection);
		float angle = FMath::Acos(dotProduct);
		if (angle > maxTiltAngle) {
			// Angle is too off.  Stop here to end the spline
			break;
		}
		currentNode = nextNode;
	}
	return currentNode;
}

TreeDataPtr TreeGenerator::DecimateNodes( TreeDataPtr tree, float maxTiltAngle )
{
	FTreeConfig optimizedConfig;
	TreeDataPtr optimizedTree(new TreeData());
	optimizedTree->Init();

	optimizedTree->GetRoot()->SetPosition(FVector(tree->GetRoot()->GetPosition()));

	TArray<NodeDecimateInfo> stack;
	stack.Push(NodeDecimateInfo(tree->GetRoot(), optimizedTree->GetRoot()));
	
	while (stack.Num() > 0) {
		NodeDecimateInfo top = stack.Pop();

		BranchNodeList unoptimizedChildren = top.unoptimizedNode->GetChildren();
		for (BranchNodePtr unoptimizedChild : unoptimizedChildren) {
			BranchNodePtr decimatedNode = DecimateNode(unoptimizedChild, maxTiltAngle);
			BranchNodePtr optimizedChild = top.optimizedNode->AddChild(decimatedNode->GetPosition());
			stack.Push(NodeDecimateInfo(decimatedNode, optimizedChild));
		}
	}
	return optimizedTree;
}

TreeDataPtr TreeGenerator::ReducePoints( TreeDataPtr tree, float tiltAngleDegrees /*= 0*/ )
{
	if (!tiltAngleDegrees) {
		tiltAngleDegrees = config.optimizationMaxTilt;
	}

	// Get the angle in radians
	float tiltAngle = tiltAngleDegrees * PI / 180.0f;

	// reduce the number of points in the mesh
	TreeDataPtr optimizedTree = DecimateNodes(tree, tiltAngle);
	optimizedTree->SetGenerationState(TreeGenerationState::ReduceDataPoints);

	// Dispose off the original tree
	tree->Dispose();

	return optimizedTree;
}

TreeDataPtr TreeGenerator::ApplySplineSmoothing(TreeDataPtr tree)
{
	if (tree->GetGenerationState() != TreeGenerationState::ReduceDataPoints) {
		return tree;
	}

	// Calculate the frenet frame for each branch node
	CalculateTreeFrenetFrame(tree);

	TreeTraverser<TreeGenerator> traverser;
	traverser.Traverse(tree, this, &TreeGenerator::_SmoothSkeletalSegment);

	tree = ReducePoints(tree, config.smoothMaxTilt);

	// Calculate the frenet frame for each branch node
	CalculateTreeFrenetFrame(tree);

	tree->SetGenerationState(TreeGenerationState::SmoothBranch);
	return tree;
}

void TreeGenerator::_SmoothSkeletalSegment( BranchNodePtr node )
{
	if (!node->GetParent().IsValid()) {
		// Ignore the root node
		return;
	}

	BranchNodePtr start = node->GetParent();
	BranchNodePtr end = node;
	FVector p0 = FVector(start->GetPosition());
	FVector p1 = FVector(end->GetPosition());
	FVector m0 = start->GetFrenet().tangent;
	FVector m1 = end->GetFrenet().tangent;

	const float segmentLength = (end->GetPosition() - start->GetPosition()).Size();

	int numSubDivisions = static_cast<int>(segmentLength / config.outerRadius);
	if (numSubDivisions < 3) {
		// Start and end points are too close to each other.  
		// Cannot sub divide further
		return;
	}

	//BranchNodeList::iterator itErase = stl::find(start->GetChildren().begin(), start->GetChildren().end(), end);
	//start->GetChildren().erase(itErase);
	start->GetChildren().Remove(end);
	BranchNodePtr previousSubNode = start;

	for (int i = 1; i < numSubDivisions - 1; i++) {
		const float t = i / (numSubDivisions - 1.0f);
		FVector position = GetPointOnSpline(t, p0, m0, p1, m1);
		BranchNodePtr currentSubNode = previousSubNode->AddChild(FVector(position));
		previousSubNode = currentSubNode;
	}

	// Link the last sub node to the end node
	previousSubNode->GetChildren().Add(end);
	end->SetParent(previousSubNode);
}

void TreeGenerator::CalculateTreeFrenetFrame(TreeDataPtr tree)
{
	TreeTraverser<TreeGenerator> traverser;
	traverser.Traverse(tree, this, &TreeGenerator::_CalculateNodeFrenetFrame);

	// The tangents are now mapped correctly.  Use the root node's reference 
	// normal and binormal to interpolate across the entire tree structure
	// Ref: http://www.cs.cmu.edu/~fp/courses/graphics/asst5/cameraMovement.pdf
	tree->GetRoot()->SetFrenet(FrenetFrame(
			FVector(1.0, 0.0, 0.0), // tangent
			FVector(0.0, 0.0, 1.0), // normal
			FVector(0.0, 1.0, 0.0) // binormal
		));

	// Traverse the tree in breadth first order
	TQueue<BranchNodePtr> queue;
	queue.Enqueue(tree->GetRoot());
	while(!queue.IsEmpty()) {
		BranchNodePtr front; 
		queue.Dequeue(front);

		// Add the children
		BranchNodeList& children = front->GetChildren();
		for (BranchNodePtr branchNode : children) {
			queue.Enqueue(branchNode);
		}

		if (!front->GetParent().IsValid()) {
			continue;
		}

		const FrenetFrame& parentFrenet = front->GetParent()->GetFrenet();
		FrenetFrame& frenet = front->GetFrenet();
		frenet.normal = FVector::CrossProduct(parentFrenet.binormal, frenet.tangent);
		frenet.normal.Normalize();
		frenet.binormal = FVector::CrossProduct(frenet.tangent, frenet.normal);
		frenet.binormal.Normalize();
		frenet.normal = FVector::CrossProduct(frenet.binormal, frenet.tangent);
		frenet.normal.Normalize();
	}
}

void TreeGenerator::_CalculateNodeFrenetFrame( BranchNodePtr node )
{
	FVector tangent;
	if (!node->GetParent().IsValid()) {
		tangent = FVector(1.0, 0.0, 0.0);
	} else {
		tangent = (node->GetPosition() - node->GetParent()->GetPosition());
		tangent.Normalize();
		// Take the average vector of outgoing branches
		if (node->GetChildren().Num() > 0) {
			FVector outgoingVector(0, 0, 0);
			BranchNodeList& children = node->GetChildren();
			for (BranchNodePtr child : children) {
				FVector direction = child->GetPosition() - node->GetPosition();
				direction.Normalize();
				outgoingVector += direction;
			}

			outgoingVector.Normalize();
			tangent += outgoingVector;
			tangent.Normalize();
		}
	}
	node->GetFrenet().tangent = tangent;
}

FVector TreeGenerator::GetPointOnSpline( float t, const FVector& p0, const FVector& m0,
																 const FVector& p1, const FVector& m1 )
{
	// Clamp t between 0 and 1
	t = FMath::Clamp(t, 0.0f, 1.0f);

	const float t2 = t * t;
	const float t3 = t2 * t;
	const float h00 = 2 * t3 - 3 * t2 + 1;
	const float h10 = t3 - 2 * t2 + t;
	const float h01 = -2 * t3 + 3 * t2;
	const float h11 = t3 - t2;

	return p0 * h00 +
		m0 * h10 + 
		p1 * h01 +
		m1 * h11;
}

TreeDataPtr TreeGenerator::BuildMesh( TreeDataPtr tree )
{
	TreeTraverser<TreeGenerator> traverser;

	// Reset the thickness of all the branches
	traverser.Traverse(tree, this, &TreeGenerator::_ResetThickness);

	// Assign thickness to the branches.  The tip of each branch has a constant thickness
	// As two branches join, they thickness is added.   The trunk would be the thickness
	// of all the individual branches
	traverser.Traverse(tree, this, &TreeGenerator::_UpdateThickness);

	// Find the distance of each node from the root, along the skeleton
	traverser.Traverse(tree, this, &TreeGenerator::_UpdateLengthFromRoot);

	// Find the reference point of the circle
	circlePoints.Reset();
	circleNormals.Reset();
	const float radius = GetRadiusFromArea(config.branchTipArea);
	for (int i = 0; i < config.circumferenceResolution; i++) {
		const float angle = 2 * PI / config.circumferenceResolution * i;
		const FVector normal(FMath::Sin(angle), 0.0, FMath::Cos(angle));
		const FVector point = normal * radius;
		circlePoints.Add(point);
		circleNormals.Add(normal);
	}
	LineInfoList& meshData = tree->GetMeshData();
	meshData.Reset();
	branchNodeVertexIndexCounter = 0;

	// Generate the mesh vertices
	traverser.Traverse(tree, this, &TreeGenerator::_GenerateMeshVertices);

	// Generate the mesh wireframe
	traverser.Traverse(tree, this, &TreeGenerator::_GenerateMeshLines);

	// Generate the mesh triangles
	traverser.Traverse(tree, this, &TreeGenerator::_GenerateMeshTriangles);

	tree->SetGenerationState(TreeGenerationState::MeshGenerated);
	tree->SetMeshDirty(true);
	return tree;
}

void TreeGenerator::_ResetThickness( BranchNodePtr node )
{
	node->SetThicknessUnits(0);
}

void TreeGenerator::_UpdateThickness( BranchNodePtr node )
{
	if (node->GetChildren().Num() == 0) {
		BranchNodePtr branchNode = node;
		while (branchNode.IsValid()) {
			branchNode->SetThicknessUnits(branchNode->GetThicknessUnits() + 1);
			branchNode = branchNode->GetParent();
		}
	}
}

void TreeGenerator::_GenerateMeshVertices( BranchNodePtr node )
{
	node->SetVertexIndexBase(branchNodeVertexIndexCounter);
	branchNodeVertexIndexCounter = branchNodeVertexIndexCounter + config.circumferenceResolution;
	FrenetFrame& frenet = node->GetFrenet();
	FMatrix RotationMat(frenet.binormal, frenet.tangent, frenet.normal, FVector::ZeroVector);
	FQuat rotation = RotationMat.ToQuat();

	node->GetVertices().Reset();
	// Insert circle lines for this node
	const float circleArea = node->GetThicknessUnits() * config.branchTipArea;
	const float radius = GetRadiusFromArea(circleArea);
	for (int i = 0; i < config.circumferenceResolution; i++) {
		const FVector point = node->GetPosition() + rotation * (circlePoints[i] * radius);
		const FVector normal = rotation * circleNormals[i];
		node->GetVertices().Add(point);
		TreeVertexData vertex;
		vertex.position = point;
		vertex.normal = normal;
		vertex.uv.X = i / float(config.circumferenceResolution);
		vertex.uv.Y = node->GetDistanceFromRoot() / 100.0f;
		node->GetTree()->GetTrunkGeometry().vertices.Add(vertex);
	}
}

void TreeGenerator::_GenerateMeshLines( BranchNodePtr node )
{
	for (int i = 0; i < config.circumferenceResolution; i++) {
		int startIndex = node->GetVertexIndexBase() + i;
		int endIndex = node->GetVertexIndexBase() + (i + 1) % config.circumferenceResolution;
		LineInfo line(startIndex, endIndex);
		node->GetTree()->GetMeshData().Add(line);
		if (node->GetParent().IsValid()) {
			int lowerEndIndex = node->GetParent()->GetVertexIndexBase() + i;
			node->GetTree()->GetMeshData().Add(LineInfo(startIndex, lowerEndIndex));
		}
	}
}

void TreeGenerator::_GenerateMeshTriangles( BranchNodePtr node )
{
	// Generate trunk geometry
	{
		BranchNodePtr parent = node->GetParent();
		if (!parent.IsValid()) return;
		TriangleDataList& triangles = node->GetTree()->GetTrunkGeometry().triangles;
		for (int i = 0; i < config.circumferenceResolution; i++) {
			int v0 = node->GetVertexIndexBase() + i;
			int v1 = node->GetVertexIndexBase() + (i + 1) % config.circumferenceResolution;
			int v2 = parent->GetVertexIndexBase() + (i + 1) % config.circumferenceResolution;
			int v3 = parent->GetVertexIndexBase() + i;

			triangles.Add(TriangleData(v0, v2, v1, FVector()));
			triangles.Add(TriangleData(v0, v3, v2, FVector()));
		}
	}

	// Generate Leaves geometry
	{
		// Check if we need to generate leaves in this node
		if (node->GetChildren().Num() == 0) {
			TreeGeometry& leaves = node->GetTree()->GetLeavesGeometry();
			// This is a leaf node. Generate leaves
			BranchNodePtr parent = node->GetParent();
			if (!parent.IsValid()) return;

			int startIndex = leaves.vertices.Num();
			FVector direction = node->GetPosition() - parent->GetPosition();
			direction.Normalize();
			const FVector leafSideRef(1, 0, 0);
			const FVector leafNormalRef(0, 1, 0);
			
			//FQuat quat = Quat::rotation(leafSideRef, direction);
			FQuat quat = FQuat((direction - leafSideRef).Rotation());			////// TODO: CHECK ME

			const float leafSize = 1.0f;
			const float halfLeafSize = leafSize / 2.0f;
			const FVector leafVerts[] = {
				FVector(-halfLeafSize, 0, -halfLeafSize),
				FVector(halfLeafSize, 0, -halfLeafSize),
				FVector(halfLeafSize, 0, halfLeafSize),
				FVector(-halfLeafSize, 0, halfLeafSize)
			};
			const FVector2D leafUVs[] = {
				FVector2D(0, 0),
				FVector2D(1, 0),
				FVector2D(1, 1),
				FVector2D(0, 1)
			};
			for (int i = 0; i < 4; i++) {
				TreeVertexData vertex;
				vertex.position = (quat * leafVerts[i]) + node->GetPosition();
				vertex.uv = leafUVs[i];
				vertex.normal = (quat * leafNormalRef);
				leaves.vertices.Add(vertex);
			}
			const int indicesRef[] = { 0, 2, 1, 0, 3, 2 };
			const int v0 = 0 + startIndex;
			const int v1 = 1 + startIndex;
			const int v2 = 2 + startIndex;
			const int v3 = 3 + startIndex;
			const FVector p0 = FVector(leaves.vertices[v0].position);
			const FVector p1 = FVector(leaves.vertices[v1].position);
			const FVector p2 = FVector(leaves.vertices[v2].position);
			const FVector p3 = FVector(leaves.vertices[v3].position);
			const FVector center0 = FVector(p0 + p2 + p1) / 3.0f;
			const FVector center1 = FVector(p0 + p3 + p2) / 3.0f;

			leaves.triangles.Add(TriangleData(v0, v2, v1, FVector(center0)));
			leaves.triangles.Add(TriangleData(v0, v3, v2, FVector(center0)));
		}
	}

}

void TreeGenerator::_UpdateLengthFromRoot( BranchNodePtr node )
{
	BranchNodePtr parent = node->GetParent();
	if (!parent.IsValid()) return;
	float distance = (node->GetPosition() - parent->GetPosition()).Size();
	node->SetDistanceFromRoot(parent->GetDistanceFromRoot() + distance);
}

float TreeGenerator::GetRadiusFromArea( float area )
{
	return FMath::Sqrt(area / PI);
}

