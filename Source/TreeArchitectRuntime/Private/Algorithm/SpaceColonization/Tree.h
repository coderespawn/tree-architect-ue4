//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

#include <deque>


class TREEARCHITECTRUNTIME_API PointCloudPoint {
public:
	FORCEINLINE FVector GetPosition() const { return position; }
	FORCEINLINE void SetPosition(FVector val) { position = val; }

protected:
	FVector position;
};

typedef TSharedPtr<PointCloudPoint> PointCloudPointPtr;

class BranchNode;
typedef TSharedPtr<BranchNode> BranchNodePtr;

class TREEARCHITECTRUNTIME_API SpaceNode : public PointCloudPoint {
public:
	SpaceNode(const FVector& position, BranchNodePtr closestNode);
	FORCEINLINE BranchNodePtr GetClosestNode() const { return closestNode; }
	FORCEINLINE void SetClosestNode(BranchNodePtr val) { closestNode = val; }
	FORCEINLINE float GetDistanceSqToClosestNode() const { return distanceSqToClosestNode; }
	FORCEINLINE void SetDistanceSqToClosestNode(float val) { distanceSqToClosestNode = val; }

	void UpdateClosestNode(BranchNodePtr node);

private:
	BranchNodePtr closestNode;
	float distanceSqToClosestNode;
};
typedef TSharedPtr<SpaceNode> SpaceNodePtr;
typedef TArray<SpaceNodePtr> SpaceNodeList;

typedef TArray<FVector> Vector3List;

struct TREEARCHITECTRUNTIME_API FrenetFrame {
	FVector tangent;
	FVector normal;
	FVector binormal;

	FrenetFrame() {}
	FrenetFrame(const FVector& tangent, const FVector& normal, const FVector& binormal) 
		: tangent(tangent), normal(normal), binormal(binormal) {}
};

class TreeData;
typedef TSharedPtr<TreeData> TreeDataPtr;


typedef TArray<BranchNodePtr> BranchNodeList;

class TREEARCHITECTRUNTIME_API BranchNode : public PointCloudPoint, public TSharedFromThis<BranchNode> {
public:
	BranchNode(BranchNodePtr parent, TreeDataPtr tree, const FVector& position);

	void Dispose();

	/** 
		* Grows the branch along the [offset]
		* This is done by adding a child node
		* Returns the new branch node that was created
		*/
	BranchNodePtr Grow(float outerDiameter);

	BranchNodePtr AddChild(const FVector& childPosition);

	/**
	* Initializes the branch node for the next growth cycle
	*/
	void InitializeForNextGrowth();

	int GetId() const { return id; }
	void SetId(int val) { id = val; }
	FrenetFrame& GetFrenet() { return frenet; }
	void SetFrenet(FrenetFrame val) { frenet = val; }
	BranchNodePtr GetParent() const { return parent; }
	void SetParent(BranchNodePtr val) { parent = val; }
	TreeDataPtr GetTree() const { return tree; }
	void SetTree(TreeDataPtr val) { tree = val; }
	BranchNodeList& GetChildren() { return children; }
	void SetChildren(BranchNodeList val) { children = val; }
	float GetThicknessUnits() const { return thicknessUnits; }
	void SetThicknessUnits(float val) { thicknessUnits = val; }
	Vector3List& GetVertices() { return vertices; }
	void SetVertices(Vector3List val) { vertices = val; }
	int GetVertexIndexBase() const { return vertexIndexBase; }
	void SetVertexIndexBase(int val) { vertexIndexBase = val; }
	const FVector& GetAttractionVector() const { return attractionVector; }
	void SetAttractionVector(FVector val) { attractionVector = val; }
	float GetDistanceFromRoot() const { return distanceFromRoot; }
	void SetDistanceFromRoot(float val) { distanceFromRoot = val; }

private:
	static int _idCounter;
	int id;

	/** The frenet frame for this point on the branch node */
	FrenetFrame frenet;

	/** This branch's parent node. Every node will have a parent except the root */
	BranchNodePtr parent;

	/** The tree reference that contains this branch node */
	TreeDataPtr tree;


	/**
		* The child branch nodes.   If the branch splits form this node, the 
		* size would be greater than one depending on the splits
		* If this node is in the middle of a branch, the size would be one
		* Tip of branches (leaf nodes) would have no children
		*/
	BranchNodeList children;
	/** 
		* The thickness of the branch size in relative units.  
		* When a branch splits, this branch size is split across the 
		* child branches
		*/
	float thicknessUnits;

	/**
		* The vertices that define this branch node
		* This vertices are of a circular cross section of the branch
		*/
	Vector3List vertices;

	/** The base of the vertex index. This is used for indexing into the vertex buffer */
	int vertexIndexBase;

	FVector attractionVector;

	/** The distance of from the root node to this node, along it's skeleton */
	float distanceFromRoot;
};
	
struct LineInfo {
	LineInfo() : startIndex(-1), endIndex(-1) {}
	LineInfo(int startIndex, int endIndex, const FColor& color = FColor(1, 1, 1)) 
		: startIndex(startIndex), endIndex(endIndex), color(color) {}
	size_t startIndex;
	size_t endIndex;
	FColor color;
};
typedef TArray<LineInfo> LineInfoList;

enum class TreeGenerationState {
	Uninitialized,
	AttractionPoints,
	SkeletonGenerationProgress,
	SkeletonGenerated,
	ReduceDataPoints,
	SmoothBranch,
	MeshGenerated
};

struct TriangleData {
	TriangleData(uint16 v0, uint16 v1, uint16 v2, const FVector& center) : center(center), v0(v0), v1(v1), v2(v2), distanceFromCamera(0) {}
	TriangleData() {}
	uint32 v0;
	uint32 v1;
	uint32 v2;
	FVector center;
	float distanceFromCamera;
	bool operator<(const TriangleData& other);
};
typedef TArray<TriangleData> TriangleDataList;

struct TreeVertexData {
	FVector position;
	FVector normal;
	FVector2D uv;
};
typedef TArray<TreeVertexData> TreeVertexDataList;

struct TreeGeometry {
	TreeVertexDataList vertices;
	TriangleDataList triangles;
};

/** Data model for the tree. Holds all the data for representing the tree */
class TREEARCHITECTRUNTIME_API TreeData : public TSharedFromThis<TreeData> {
public:
	/** Call the Init function after creation of the object */
	TreeData();

	/** Call this init function after the creation of the object */
	void Init();
	void Dispose();

	FORCEINLINE SpaceNodeList& GetSpacePoints() { return spacePoints; }
	FORCEINLINE void SetSpacePoints(const SpaceNodeList& val) { spacePoints = val; }
	FORCEINLINE BranchNodePtr GetRoot() const { return root; }
	FORCEINLINE void SetRoot(BranchNodePtr val) { root = val; }
	FORCEINLINE TreeGenerationState GetGenerationState() const { return generationState; }
	FORCEINLINE void SetGenerationState(TreeGenerationState val) { generationState = val; }
	FORCEINLINE LineInfoList& GetMeshData() { return meshData; }
	FORCEINLINE void SetMeshData(const LineInfoList& val) { meshData = val; }
	FORCEINLINE bool IsMeshDirty() const { return meshDirty; }
	FORCEINLINE void SetMeshDirty(bool val) { meshDirty = val; }
	FORCEINLINE TreeGeometry& GetTrunkGeometry() { return trunkGeometry; }
	FORCEINLINE TreeGeometry& GetLeavesGeometry() { return leavesGeometry; }

private:
	void _DisposeBranchNode(BranchNodePtr node);

private:
	SpaceNodeList spacePoints;
	BranchNodePtr root;
	TreeGenerationState generationState;
	LineInfoList meshData;
	bool meshDirty;
	TreeGeometry trunkGeometry;
	TreeGeometry leavesGeometry;
};


template<typename T> 
class TreeTraverser {
public:
	typedef void (T::*OnNodeVisited)(BranchNodePtr node);
	void Traverse(TreeDataPtr tree, T* owner, OnNodeVisited onNodeVisited) {
		TArray<BranchNodePtr> stack;
		
		stack.Push(tree->GetRoot());
		while (stack.Num() > 0) {
			BranchNodePtr top = stack.Pop();
			(owner->*onNodeVisited)(top);

			BranchNodeList& children = top->GetChildren();
			for (BranchNodePtr child : children) {
				stack.Push(child);
			}
		}
	}
};
