//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeGenerator.h"
#include "Tree.h"

//
//#include <math.h>
//using namespace respawn::renderer;
//using namespace respawn::math;
//using namespace respawn::system;
//using namespace respawn;
//using namespace minitree;
//using namespace stl;
//
//minitree::TreeConfig::TreeConfig() : numSpacePoints(1000), minSpacePoints(50), attractionPointRadius(2), 
//	heightOffset(3.5f), innerRadius(0.025f), outerRadius(0.050f), killDistance(0.2f),
//	smoothingStrength(0.6f), branchTipArea(0.05f), optimizationMaxTilt(3.0f),
//	smoothMaxTilt(0.30f), circumferenceResolution(6)
//{
//
//}
//
//float minitree::TreeConfig::GetOuterDiameter() const
//{
//	return outerRadius * 2;
//}
//
//
//minitree::TreeDataPtr minitree::TreeGenerator::Generate( const TreeConfig& config )
//{
//	this->config = config;
//	TreeDataPtr tree = TreeDataPtr(new TreeData());
//	tree->Init();
//
//	// Create space point clouds for growing the tree
//	Point3 pointOnSphere;
//	SpaceNodeList& spacePoints = tree->GetSpacePoints();
//	spacePoints.clear();
//	spacePoints.reserve(config.numSpacePoints);
//
//	for (int i = 0; i < config.numSpacePoints; i++) {
//		MathUtils::GetPointOnSphere(pointOnSphere);
//		float depth = pow(MathUtils::fastrandf(), 1 / 3.0f);
//		pointOnSphere = Point3(Vector3(pointOnSphere) * (depth * config.attractionPointRadius));
//		if (pointOnSphere.getY() < 0) {
//			pointOnSphere.setY(pointOnSphere.getY() * 0.5f);
//		}
//		if (pointOnSphere.getX() > 0) {
//			pointOnSphere.setX(pointOnSphere.getX() * 1.25f);
//		} else {
//			pointOnSphere.setX(pointOnSphere.getX() * 0.75f);
//		}
//		pointOnSphere.setY(pointOnSphere.getY() + config.heightOffset);
//		SpaceNodePtr spaceNode(new SpaceNode(pointOnSphere, tree->GetRoot()));
//		spacePoints.push_back(spaceNode);
//	}
//	tree->SetGenerationState(TGS_AttractionPoints);
//
//	return tree;
//}
//
//void minitree::TreeGenerator::OnBranchNodeAdded( TreeDataPtr tree, BranchNodePtr node )
//{
//	// Traverse through all the space nodes and check if this node is closer than 
//	// the previously registered node 
//	//tree.spacePoints.forEach((SpaceNode spaceNode) => spaceNode.updateClosestNode(node));
//	SpaceNodeList& spaceNodes = tree->GetSpacePoints();
//	for (SpaceNodeList::iterator it = spaceNodes.begin(); it != spaceNodes.end(); ++it) {
//		SpaceNodePtr spaceNode = *it;
//		spaceNode->UpdateClosestNode(node);
//	}
//}
//
//void minitree::TreeGenerator::Grow( TreeDataPtr tree, int maxFrames /*= 1*/, float maxProcessingTime /*= 16*/ )
//{
//	timer->Start();
//	int framesElapsed = 0;
//	while (true) {
//		if (tree->GetGenerationState() != TGS_SkeletonGenerationProgress) {
//			// Not in the correct state (or generation is done)
//			break;
//		}
//
//		// Grow all the branch frames in the tree by one step
//		GrowBranchFrame(tree);
//
//		framesElapsed++;
//		if (maxFrames > 0 && framesElapsed >= maxFrames) {
//			// Generated the max requested frames. bail out
//			break;
//		}
//
//		if (maxProcessingTime > 0 && timer->GetElapsedTime() >= maxProcessingTime) {
//			// The max processing time has elapsed.  bail out
//			break;
//		}
//	}
//	timer->Stop();
//}
//
//minitree::TreeGenerator::TreeGenerator( const respawn::EngineContext& engineContext) 
//		: engineContext(engineContext), branchNodeVertexIndexCounter(0)
//{
//	timer = engineContext.GetPlatformSupport().CreateTimer();
//	memset(&config, 0, sizeof(config));
//}
//
//void minitree::TreeGenerator::_InitializeForNextGrowth( BranchNodePtr node )
//{
//	node->InitializeForNextGrowth();
//}
//
//void minitree::TreeGenerator::GrowBranchFrame(TreeDataPtr tree)
//{
//	if (tree->GetGenerationState() != TGS_SkeletonGenerationProgress) {
//		// Wrong state
//		return;
//	}
//
//	timer->Start();
//
//	// Initialize all the nodes in the tree for next growth frame
//	TreeTraverser<TreeGenerator> traverser;
//	traverser.Traverse(tree, this, &TreeGenerator::_InitializeForNextGrowth);
//
//	// Find the nodes that will grow in this frame
//	typedef stl::map<int, BranchNodePtr> BranchNodeMap;
//	BranchNodeMap growthNodes;
//	SpaceNodeList& spacePoints = tree->GetSpacePoints();
//	for (SpaceNodeList::iterator it = spacePoints.begin(); it != spacePoints.end(); ++it) {
//		SpaceNodePtr spaceNode = *it;
//		BranchNodePtr branchNode = spaceNode->GetClosestNode();
//		growthNodes[branchNode->GetId()] = branchNode;
//	}
//
//	// Update the attraction vector for all the branch nodes
//	Vector3 attractionVector(0, 0, 0);
//	for (SpaceNodeList::iterator it = spacePoints.begin(); it != spacePoints.end(); ++it) {
//		SpaceNodePtr spaceNode = *it;
//		BranchNodePtr branchNode = spaceNode->GetClosestNode();
//		attractionVector = normalize(spaceNode->GetPosition() - branchNode->GetPosition());
//		branchNode->SetAttractionVector(branchNode->GetAttractionVector() + attractionVector);
//	}
//
//	// Grow the tree
//	int nodesGrown = 0;
//	for (BranchNodeMap::iterator it = growthNodes.begin(); it != growthNodes.end(); ++it) {
//		BranchNodePtr node = it->second;
//		BranchNodePtr newNode = node->Grow(config.GetOuterDiameter());
//		if (newNode) {
//			nodesGrown++;
//			OnBranchNodeAdded(tree, newNode);
//		}
//	}
//
//	// Remove nearby attraction points
//	SpaceNodeList::iterator it = spacePoints.begin();
//	const float killDistance = config.killDistance;
//	const float killDistance2 = killDistance * killDistance;
//	const float innerRadius = config.innerRadius;
//	const float innerRadius2 = innerRadius * innerRadius;
//	
//	while (it != spacePoints.end()) {
//		SpaceNodePtr spacePoint = *it;
//		if (spacePoint->GetDistanceSqToClosestNode() - innerRadius2 <= killDistance2) {
//			// Point is too close. consume it
//			it = spacePoints.erase(it);
//		} else {
//			++it;
//		}
//	}
//
//	// If too few space points are left, the complete the generation process
//	if (spacePoints.size() <= config.minSpacePoints) {
//		spacePoints.clear();
//		tree->SetGenerationState(TGS_SkeletonGenerated);
//	}
//}
//
//minitree::BranchNodePtr minitree::TreeGenerator::DecimateNode( BranchNodePtr node, float maxTiltAngle )
//{
//	BranchNodeList& children = node->GetChildren();
//	if (children.size() != 1) {
//		// Do not remove this node since the branch splits here
//		return node;
//	}
//
//	if (children.size() == 0) {
//		// Reached a leaf node.  Cannot remove this
//		return node;
//	}
//
//	BranchNodePtr currentNode = children[0];
//	Vector3 referenceDirection = normalize(node->GetPosition() - currentNode->GetPosition());
//	while (currentNode->GetChildren().size() == 1) {
//		// Check if the next node is too off from the reference direction
//		BranchNodePtr nextNode = currentNode->GetChildren()[0];
//		Vector3 currentDirection = normalize(node->GetPosition() - nextNode->GetPosition());
//		
//		// Find the angle between the two vectors
//		float dotProduct = dot(currentDirection, referenceDirection);
//		float angle = acos(dotProduct);
//		if (angle > maxTiltAngle) {
//			// Angle is too off.  Stop here to end the spline
//			break;
//		}
//		currentNode = nextNode;
//	}
//	return currentNode;
//}
//
//minitree::TreeDataPtr minitree::TreeGenerator::DecimateNodes( TreeDataPtr tree, float maxTiltAngle )
//{
//	TreeConfig optimizedConfig;
//	TreeDataPtr optimizedTree(new TreeData());
//	optimizedTree->Init();
//
//	optimizedTree->GetRoot()->SetPosition(Point3(tree->GetRoot()->GetPosition()));
//
//	std::deque<NodeDecimateInfo> stack;
//	stack.push_back(NodeDecimateInfo(tree->GetRoot(), optimizedTree->GetRoot()));
//	
//	while (!stack.empty()) {
//		NodeDecimateInfo top = stack.back();
//		stack.pop_back();
//
//		BranchNodeList unoptimizedChildren = top.unoptimizedNode->GetChildren();
//		for (BranchNodeList::iterator it = unoptimizedChildren.begin(); it != unoptimizedChildren.end(); ++it) {
//			BranchNodePtr unoptimizedChild = *it;
//			BranchNodePtr decimatedNode = DecimateNode(unoptimizedChild, maxTiltAngle);
//			BranchNodePtr optimizedChild = top.optimizedNode->AddChild(decimatedNode->GetPosition());
//			stack.push_back(NodeDecimateInfo(decimatedNode, optimizedChild));
//		}
//	}
//	return optimizedTree;
//}
//
//minitree::TreeDataPtr minitree::TreeGenerator::ReducePoints( TreeDataPtr tree, float tiltAngleDegrees /*= 0*/ )
//{
//	if (!tiltAngleDegrees) {
//		tiltAngleDegrees = config.optimizationMaxTilt;
//	}
//
//	// Get the angle in radians
//	float tiltAngle = tiltAngleDegrees * MATH_PI / 180.0f;
//
//	// reduce the number of points in the mesh
//	TreeDataPtr optimizedTree = DecimateNodes(tree, tiltAngle);
//	optimizedTree->SetGenerationState(TGS_ReduceDataPoints);
//
//	// Dispose off the original tree
//	tree->Dispose();
//
//	return optimizedTree;
//}
//
//minitree::TreeDataPtr minitree::TreeGenerator::ApplySplineSmoothing(TreeDataPtr tree)
//{
//	if (tree->GetGenerationState() != TGS_ReduceDataPoints) {
//		return tree;
//	}
//
//	// Calculate the frenet frame for each branch node
//	CalculateTreeFrenetFrame(tree);
//
//	TreeTraverser<TreeGenerator> traverser;
//	traverser.Traverse(tree, this, &TreeGenerator::_SmoothSkeletalSegment);
//
//	tree = ReducePoints(tree, config.smoothMaxTilt);
//
//	// Calculate the frenet frame for each branch node
//	CalculateTreeFrenetFrame(tree);
//
//	tree->SetGenerationState(TGS_SmoothBranch);
//	return tree;
//}
//
//void minitree::TreeGenerator::_SmoothSkeletalSegment( BranchNodePtr node )
//{
//	if (!node->GetParent()) {
//		// Ignore the root node
//		return;
//	}
//
//	BranchNodePtr start = node->GetParent();
//	BranchNodePtr end = node;
//	Vector3 p0 = Vector3(start->GetPosition());
//	Vector3 p1 = Vector3(end->GetPosition());
//	Vector3 m0 = start->GetFrenet().tangent;
//	Vector3 m1 = end->GetFrenet().tangent;
//
//	const float segmentLength = length(end->GetPosition() - start->GetPosition());
//
//	int numSubDivisions = static_cast<int>(segmentLength / config.outerRadius);
//	if (numSubDivisions < 3) {
//		// Start and end points are too close to each other.  
//		// Cannot sub divide further
//		return;
//	}
//
//	BranchNodeList::iterator itErase = stl::find(start->GetChildren().begin(), start->GetChildren().end(), end);
//	start->GetChildren().erase(itErase);
//	BranchNodePtr previousSubNode = start;
//
//	for (int i = 1; i < numSubDivisions - 1; i++) {
//		const float t = i / (numSubDivisions - 1.0f);
//		Vector3 position = GetPointOnSpline(t, p0, m0, p1, m1);
//		BranchNodePtr currentSubNode = previousSubNode->AddChild(Point3(position));
//		previousSubNode = currentSubNode;
//	}
//
//	// Link the last sub node to the end node
//	previousSubNode->GetChildren().push_back(end);
//	end->SetParent(previousSubNode);
//}
//
//void minitree::TreeGenerator::CalculateTreeFrenetFrame(TreeDataPtr tree)
//{
//	TreeTraverser<TreeGenerator> traverser;
//	traverser.Traverse(tree, this, &TreeGenerator::_CalculateNodeFrenetFrame);
//
//	// The tangents are now mapped correctly.  Use the root node's reference 
//	// normal and binormal to interpolate across the entire tree structure
//	// Ref: http://www.cs.cmu.edu/~fp/courses/graphics/asst5/cameraMovement.pdf
//	tree->GetRoot()->SetFrenet(FrenetFrame(
//			Vector3(0.0, 1.0, 0.0), // tangent
//			Vector3(-1.0, 0.0, 0.0), // normal
//			Vector3(0.0, 0.0, 1.0) // binormal
//		));
//
//	// Traverse the tree in breadth first order
//	std::deque<BranchNodePtr> queue;
//	queue.push_back(tree->GetRoot());
//	while(!queue.empty()) {
//		BranchNodePtr front = queue.front(); 
//		queue.pop_front();
//
//		// Add the children
//		BranchNodeList& children = front->GetChildren();
//		for (BranchNodeList::iterator it = children.begin(); it != children.end(); ++it) {
//			queue.push_back(*it);
//		}
//
//		if (!front->GetParent()) {
//			continue;
//		}
//
//		const FrenetFrame& parentFrenet = front->GetParent()->GetFrenet();
//		FrenetFrame& frenet = front->GetFrenet();
//		frenet.normal = normalize(cross(parentFrenet.binormal, frenet.tangent));
//		frenet.binormal = normalize(cross(frenet.tangent, frenet.normal));
//		frenet.normal = cross(frenet.binormal, frenet.tangent);
//	}
//}
//
//void minitree::TreeGenerator::_CalculateNodeFrenetFrame( BranchNodePtr node )
//{
//	Vector3 tangent;
//	if (!node->GetParent()) {
//		tangent = Vector3(0.0, 1.0, 0.0);
//	} else {
//		tangent = normalize(node->GetPosition() - node->GetParent()->GetPosition());
//		// Take the average vector of outgoing branches
//		if (node->GetChildren().size() > 0) {
//			Vector3 outgoingVector(0, 0, 0);
//			BranchNodeList& children = node->GetChildren();
//			for (BranchNodeList::iterator it = children.begin(); it != children.end(); ++it) {
//				BranchNodePtr child = *it;
//				outgoingVector += normalize(child->GetPosition() - node->GetPosition());
//			}
//
//			outgoingVector = normalize(outgoingVector);
//			tangent = normalize(tangent + outgoingVector);
//		}
//	}
//	node->GetFrenet().tangent = tangent;
//}
//
//respawn::math::Vector3 minitree::TreeGenerator::GetPointOnSpline( float t, const respawn::math::Vector3& p0, const respawn::math::Vector3& m0,
//																 const respawn::math::Vector3& p1, const respawn::math::Vector3& m1 )
//{
//	// Clamp t between 0 and 1
//	t = MathUtils::clamp(t, 0, 1);
//
//	const float t2 = t * t;
//	const float t3 = t2 * t;
//	const float h00 = 2 * t3 - 3 * t2 + 1;
//	const float h10 = t3 - 2 * t2 + t;
//	const float h01 = -2 * t3 + 3 * t2;
//	const float h11 = t3 - t2;
//
//	return p0 * h00 +
//		m0 * h10 + 
//		p1 * h01 +
//		m1 * h11;
//}
//
//minitree::TreeDataPtr minitree::TreeGenerator::BuildMesh( TreeDataPtr tree )
//{
//	TreeTraverser<TreeGenerator> traverser;
//
//	// Reset the thickness of all the branches
//	traverser.Traverse(tree, this, &TreeGenerator::_ResetThickness);
//
//	// Assign thickness to the branches.  The tip of each branch has a constant thickness
//	// As two branches join, they thickness is added.   The trunk would be the thickness
//	// of all the individual branches
//	traverser.Traverse(tree, this, &TreeGenerator::_UpdateThickness);
//
//	// Find the distance of each node from the root, along the skeleton
//	traverser.Traverse(tree, this, &TreeGenerator::_UpdateLengthFromRoot);
//
//	// Find the reference point of the circle
//	circlePoints.clear();
//	circleNormals.clear();
//	const float radius = GetRadiusFromArea(config.branchTipArea);
//	for (int i = 0; i < config.circumferenceResolution; i++) {
//		const float angle = 2 * MATH_PI / config.circumferenceResolution * i;
//		const Vector3 normal(sinf(angle), 0.0, cosf(angle));
//		const Vector3 point = normal * radius;
//		circlePoints.push_back(point);
//		circleNormals.push_back(normal);
//	}
//	LineInfoList& meshData = tree->GetMeshData();
//	meshData.clear();
//	branchNodeVertexIndexCounter = 0;
//
//	// Generate the mesh vertices
//	traverser.Traverse(tree, this, &TreeGenerator::_GenerateMeshVertices);
//
//	// Generate the mesh wireframe
//	traverser.Traverse(tree, this, &TreeGenerator::_GenerateMeshLines);
//
//	// Generate the mesh triangles
//	traverser.Traverse(tree, this, &TreeGenerator::_GenerateMeshTriangles);
//
//	tree->SetGenerationState(TGS_MeshGenerated);
//	tree->SetMeshDirty(true);
//	return tree;
//}
//
//void minitree::TreeGenerator::_ResetThickness( BranchNodePtr node )
//{
//	node->SetThicknessUnits(0);
//}
//
//void minitree::TreeGenerator::_UpdateThickness( BranchNodePtr node )
//{
//	if (node->GetChildren().size() == 0) {
//		BranchNodePtr branchNode = node;
//		while (branchNode) {
//			branchNode->SetThicknessUnits(branchNode->GetThicknessUnits() + 1);
//			branchNode = branchNode->GetParent();
//		}
//	}
//}
//
//void minitree::TreeGenerator::_GenerateMeshVertices( BranchNodePtr node )
//{
//	node->SetVertexIndexBase(branchNodeVertexIndexCounter);
//	branchNodeVertexIndexCounter = branchNodeVertexIndexCounter + config.circumferenceResolution;
//	FrenetFrame& frenet = node->GetFrenet();
//	Matrix3 rotation(frenet.binormal, frenet.tangent, frenet.normal);
//
//	node->GetVertices().clear();
//	// Insert circle lines for this node
//	const float circleArea = node->GetThicknessUnits() * config.branchTipArea;
//	const float radius = GetRadiusFromArea(circleArea);
//	for (int i = 0; i < config.circumferenceResolution; i++) {
//		const Point3 point = node->GetPosition() + rotation * (circlePoints[i] * radius);
//		const Vector3 normal = rotation * circleNormals[i];
//		node->GetVertices().push_back(point);
//		TreeVertexData vertex;
//		vertex.position = point;
//		vertex.normal = normal;
//		vertex.uv.x = i / float(config.circumferenceResolution);
//		vertex.uv.y = node->GetDistanceFromRoot();
//		node->GetTree()->GetTrunkGeometry().vertices.push_back(vertex);
//	}
//}
//
//void minitree::TreeGenerator::_GenerateMeshLines( BranchNodePtr node )
//{
//	for (int i = 0; i < config.circumferenceResolution; i++) {
//		int startIndex = node->GetVertexIndexBase() + i;
//		int endIndex = node->GetVertexIndexBase() + (i + 1) % config.circumferenceResolution;
//		LineInfo line(startIndex, endIndex);
//		node->GetTree()->GetMeshData().push_back(line);
//		if (node->GetParent()) {
//			int lowerEndIndex = node->GetParent()->GetVertexIndexBase() + i;
//			node->GetTree()->GetMeshData().push_back(LineInfo(startIndex, lowerEndIndex));
//		}
//	}
//}
//
//void minitree::TreeGenerator::_GenerateMeshTriangles( BranchNodePtr node )
//{
//	// Generate trunk geometry
//	{
//		BranchNodePtr parent = node->GetParent();
//		if (!parent) return;
//		TriangleDataList& triangles = node->GetTree()->GetTrunkGeometry().triangles;
//		for (int i = 0; i < config.circumferenceResolution; i++) {
//			int v0 = node->GetVertexIndexBase() + i;
//			int v1 = node->GetVertexIndexBase() + (i + 1) % config.circumferenceResolution;
//			int v2 = parent->GetVertexIndexBase() + (i + 1) % config.circumferenceResolution;
//			int v3 = parent->GetVertexIndexBase() + i;
//
//			triangles.push_back(TriangleData(v0, v2, v1, Point3()));
//			triangles.push_back(TriangleData(v0, v3, v2, Point3()));
//		}
//	}
//
//	// Generate Leaves geometry
//	{
//		// Check if we need to generate leaves in this node
//		if (node->GetChildren().size() == 0) {
//			TreeGeometry& leaves = node->GetTree()->GetLeavesGeometry();
//			// This is a leaf node. Generate leaves
//			BranchNodePtr parent = node->GetParent();
//			if (!parent) return;
//
//			int startIndex = leaves.vertices.size();
//			Vector3 direction = normalize(node->GetPosition() - parent->GetPosition());
//			const Vector3 leafSideRef(1, 0, 0);
//			const Vector3 leafNormalRef(0, 1, 0);
//			Quat quat = Quat::rotation(leafSideRef, direction);
//			const float leafSize = 1.0f;
//			const float halfLeafSize = leafSize / 2.0f;
//			const Vector3 leafVerts[] = {
//				Vector3(-halfLeafSize, 0, -halfLeafSize),
//				Vector3(halfLeafSize, 0, -halfLeafSize),
//				Vector3(halfLeafSize, 0, halfLeafSize),
//				Vector3(-halfLeafSize, 0, halfLeafSize)
//			};
//			const Vector2 leafUVs[] = {
//				Vector2(0, 0),
//				Vector2(1, 0),
//				Vector2(1, 1),
//				Vector2(0, 1)
//			};
//			for (int i = 0; i < 4; i++) {
//				TreeVertexData vertex;
//				vertex.position = rotate(quat, leafVerts[i]) + node->GetPosition();
//				vertex.uv = leafUVs[i];
//				vertex.normal = rotate(quat, leafNormalRef);
//				leaves.vertices.push_back(vertex);
//			}
//			const int indicesRef[] = { 0, 2, 1, 0, 3, 2 };
//			const int v0 = 0 + startIndex;
//			const int v1 = 1 + startIndex;
//			const int v2 = 2 + startIndex;
//			const int v3 = 3 + startIndex;
//			const Vector3 p0 = Vector3(leaves.vertices[v0].position);
//			const Vector3 p1 = Vector3(leaves.vertices[v1].position);
//			const Vector3 p2 = Vector3(leaves.vertices[v2].position);
//			const Vector3 p3 = Vector3(leaves.vertices[v3].position);
//			const Vector3 center0 = Vector3(p0 + p2 + p1) / 3.0f;
//			const Vector3 center1 = Vector3(p0 + p3 + p2) / 3.0f;
//
//			leaves.triangles.push_back(TriangleData(v0, v2, v1, Point3(center0)));
//			leaves.triangles.push_back(TriangleData(v0, v3, v2, Point3(center0)));
//		}
//	}
//
//}
//
//void minitree::TreeGenerator::_UpdateLengthFromRoot( BranchNodePtr node )
//{
//	BranchNodePtr parent = node->GetParent();
//	if (!parent) return;
//	float distance = length(node->GetPosition() - parent->GetPosition());
//	node->SetDistanceFromRoot(parent->GetDistanceFromRoot() + distance);
//}
//
//float minitree::TreeGenerator::GetRadiusFromArea( float area )
//{
//	return sqrtf(area / MATH_PI);
//}
//
