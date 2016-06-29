//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

//#include <deque>
//
//
//
//namespace minitree {
//
//
//	class GAME_API PointCloudPoint {
//	public:
//		respawn::math::Point3 GetPosition() const { return position; }
//		void SetPosition(respawn::math::Point3 val) { position = val; }
//
//	protected:
//		respawn::math::Point3 position;
//	};
//	typedef boost::shared_ptr<PointCloudPoint> PointCloudPointPtr;
//
//	class BranchNode;
//	typedef boost::shared_ptr<BranchNode> BranchNodePtr;
//
//	class GAME_API SpaceNode : public PointCloudPoint {
//	public:
//		SpaceNode(const respawn::math::Point3& position, BranchNodePtr closestNode);
//		minitree::BranchNodePtr GetClosestNode() const { return closestNode; }
//		void SetClosestNode(minitree::BranchNodePtr val) { closestNode = val; }
//		float GetDistanceSqToClosestNode() const { return distanceSqToClosestNode; }
//		void SetDistanceSqToClosestNode(float val) { distanceSqToClosestNode = val; }
//
//		void UpdateClosestNode(BranchNodePtr node);
//
//	private:
//		BranchNodePtr closestNode;
//		float distanceSqToClosestNode;
//	};
//	typedef boost::shared_ptr<SpaceNode> SpaceNodePtr;
//	typedef stl::vector<SpaceNodePtr> SpaceNodeList;
//
//	typedef stl::vector<respawn::math::Point3> Point3List;
//	typedef stl::vector<respawn::math::Vector3> Vector3List;
//
//	struct GAME_API FrenetFrame {
//		respawn::math::Vector3 tangent;
//		respawn::math::Vector3 normal;
//		respawn::math::Vector3 binormal;
//
//		FrenetFrame() {}
//		FrenetFrame(const respawn::math::Vector3& tangent, const respawn::math::Vector3& normal, const respawn::math::Vector3& binormal) 
//			: tangent(tangent), normal(normal), binormal(binormal) {}
//	};
//
//	class TreeData;
//	typedef boost::shared_ptr<TreeData> TreeDataPtr;
//
//
//	typedef stl::vector<BranchNodePtr> BranchNodeList;
//
//	class GAME_API BranchNode : public PointCloudPoint, public boost::enable_shared_from_this<BranchNode> {
//	public:
//		BranchNode(BranchNodePtr parent, TreeDataPtr tree, const respawn::math::Point3& position);
//
//		void Dispose();
//
//		/** 
//		 * Grows the branch along the [offset]
//		 * This is done by adding a child node
//		 * Returns the new branch node that was created
//		 */
//		BranchNodePtr Grow(float outerDiameter);
//
//		BranchNodePtr AddChild(const respawn::math::Point3& childPosition);
//
//		/**
//		* Initializes the branch node for the next growth cycle
//		*/
//		void InitializeForNextGrowth();
//
//		int GetId() const { return id; }
//		void SetId(int val) { id = val; }
//		minitree::FrenetFrame& GetFrenet() { return frenet; }
//		void SetFrenet(minitree::FrenetFrame val) { frenet = val; }
//		minitree::BranchNodePtr GetParent() const { return parent; }
//		void SetParent(minitree::BranchNodePtr val) { parent = val; }
//		minitree::TreeDataPtr GetTree() const { return tree; }
//		void SetTree(minitree::TreeDataPtr val) { tree = val; }
//		BranchNodeList& GetChildren() { return children; }
//		void SetChildren(BranchNodeList val) { children = val; }
//		float GetThicknessUnits() const { return thicknessUnits; }
//		void SetThicknessUnits(float val) { thicknessUnits = val; }
//		minitree::Point3List& GetVertices() { return vertices; }
//		void SetVertices(minitree::Point3List val) { vertices = val; }
//		int GetVertexIndexBase() const { return vertexIndexBase; }
//		void SetVertexIndexBase(int val) { vertexIndexBase = val; }
//		const respawn::math::Vector3& GetAttractionVector() const { return attractionVector; }
//		void SetAttractionVector(respawn::math::Vector3 val) { attractionVector = val; }
//		float GetDistanceFromRoot() const { return distanceFromRoot; }
//		void SetDistanceFromRoot(float val) { distanceFromRoot = val; }
//
//	private:
//		static int _idCounter;
//		int id;
//
//		/** The frenet frame for this point on the branch node */
//		FrenetFrame frenet;
//
//		/** This branch's parent node. Every node will have a parent except the root */
//		BranchNodePtr parent;
//
//		/** The tree reference that contains this branch node */
//		TreeDataPtr tree;
//
//
//		/**
//		 * The child branch nodes.   If the branch splits form this node, the 
//		 * size would be greater than one depending on the splits
//		 * If this node is in the middle of a branch, the size would be one
//		 * Tip of branches (leaf nodes) would have no children
//		 */
//		BranchNodeList children;
//		/** 
//		 * The thickness of the branch size in relative units.  
//		 * When a branch splits, this branch size is split across the 
//		 * child branches
//		 */
//		float thicknessUnits;
//
//		/**
//		 * The vertices that define this branch node
//		 * This vertices are of a circular cross section of the branch
//		 */
//		Point3List vertices;
//
//		/** The base of the vertex index. This is used for indexing into the vertex buffer */
//		int vertexIndexBase;
//
//		respawn::math::Vector3 attractionVector;
//
//		/** The distance of from the root node to this node, along it's skeleton */
//		float distanceFromRoot;
//	};
//	
//	struct LineInfo {
//		LineInfo() : startIndex(-1), endIndex(-1) {}
//		LineInfo(int startIndex, int endIndex, const respawn::renderer::Color& color = respawn::renderer::Color(1, 1, 1, 0)) 
//			: startIndex(startIndex), endIndex(endIndex), color(color) {}
//		size_t startIndex;
//		size_t endIndex;
//		respawn::renderer::Color color;
//	};
//	typedef stl::vector<LineInfo> LineInfoList;
//
//	enum TreeGenerationState {
//		TGS_Uninitialized,
//		TGS_AttractionPoints,
//		TGS_SkeletonGenerationProgress,
//		TGS_SkeletonGenerated,
//		TGS_ReduceDataPoints,
//		TGS_SmoothBranch,
//		TGS_MeshGenerated
//	};
//
//	struct TriangleData {
//		TriangleData(uint16 v0, uint16 v1, uint16 v2, const respawn::math::Point3& center) : center(center), v0(v0), v1(v1), v2(v2), distanceFromCamera(0) {}
//		TriangleData() {}
//		uint16 v0;
//		uint16 v1;
//		uint16 v2;
//		respawn::math::Point3 center;
//		float distanceFromCamera;
//		bool operator<(const TriangleData& other);
//	};
//	typedef std::vector<TriangleData> TriangleDataList;
//
//	struct TreeVertexData {
//		respawn::math::Point3 position;
//		respawn::math::Vector3 normal;
//		respawn::math::Vector2 uv;
//	};
//	typedef stl::vector<TreeVertexData> TreeVertexDataList;
//
//	struct TreeGeometry {
//		TreeVertexDataList vertices;
//		TriangleDataList triangles;
//	};
//
//	/** Data model for the tree. Holds all the data for representing the tree */
//	class GAME_API TreeData : public boost::enable_shared_from_this<TreeData> {
//	public:
//		/** Call the Init function after creation of the object */
//		TreeData();
//
//		/** Call this init function after the creation of the object */
//		void Init();
//		void Dispose();
//
//		minitree::SpaceNodeList& GetSpacePoints() { return spacePoints; }
//		void SetSpacePoints(const minitree::SpaceNodeList& val) { spacePoints = val; }
//		minitree::BranchNodePtr GetRoot() const { return root; }
//		void SetRoot(minitree::BranchNodePtr val) { root = val; }
//		TreeGenerationState GetGenerationState() const { return generationState; }
//		void SetGenerationState(TreeGenerationState val) { generationState = val; }
//		LineInfoList& GetMeshData() { return meshData; }
//		void SetMeshData(const LineInfoList& val) { meshData = val; }
//		bool IsMeshDirty() const { return meshDirty; }
//		void SetMeshDirty(bool val) { meshDirty = val; }
//		minitree::TreeGeometry& GetTrunkGeometry() { return trunkGeometry; }
//		minitree::TreeGeometry& GetLeavesGeometry() { return leavesGeometry; }
//
//	private:
//		void _DisposeBranchNode(BranchNodePtr node);
//
//	private:
//		SpaceNodeList spacePoints;
//		BranchNodePtr root;
//		TreeGenerationState generationState;
//		LineInfoList meshData;
//		bool meshDirty;
//		TreeGeometry trunkGeometry;
//		TreeGeometry leavesGeometry;
//	};
//
//
//	template<typename T> 
//	class TreeTraverser {
//	public:
//		typedef void (T::*OnNodeVisited)(BranchNodePtr node);
//		void Traverse(TreeDataPtr tree, T* owner, OnNodeVisited onNodeVisited) {
//			std::deque<BranchNodePtr> stack;
//			stack.push_back(tree->GetRoot());
//			while (!stack.empty()) {
//				BranchNodePtr top = stack.back(); 
//				stack.pop_back();
//				(owner->*onNodeVisited)(top);
//
//				BranchNodeList& children = top->GetChildren();
//				for (BranchNodeList::iterator it = children.begin(); it != children.end(); ++it) {
//					BranchNodePtr child = *it;
//					stack.push_back(child);
//				}
//			}
//		}
//	};
//
//}
