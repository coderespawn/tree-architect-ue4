//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

//
//namespace minitree {
//
//	class Tree;
//	struct TreeConfig;
//
//	class TreeData;
//	typedef boost::shared_ptr<TreeData> TreeDataPtr; 
//
//	class BranchNode;
//	typedef boost::shared_ptr<BranchNode> BranchNodePtr;
//
//
//	struct GAME_API TreeConfig {
//		int numSpacePoints;
//		float minSpacePoints;
//		float attractionPointRadius;
//		float heightOffset;
//		float innerRadius;
//		float outerRadius;
//		float killDistance;
//		float smoothingStrength;
//		float branchTipArea;
//		int circumferenceResolution;
//
//		// The max tilt angle in degrees while performing a data point reduction
//		float optimizationMaxTilt;
//
//		// The max tilt angle in degrees after performing a spline smoothing
//		float smoothMaxTilt;
//
//		TreeConfig();
//		float GetOuterDiameter() const;
//	};
//
//	typedef stl::vector<respawn::math::Vector3> Vector3List;
//
//	class GAME_API TreeGenerator {
//	public:
//		TreeGenerator(const respawn::EngineContext& engineContext);
//
//		/** Generates the branch structure and returns the root node */
//		TreeDataPtr Generate(const TreeConfig& config);
//  
//		/**
//		* Called when a new branch node is added to the tree
//		* This is usually done during the tree growth
//		*/
//		void OnBranchNodeAdded(TreeDataPtr tree, BranchNodePtr node);
//
//		/** 
//		 * Grows the tree by maxFrames or till the maxProcessingTime, in seconds, has elapsed 
//		 * If maxFrames is 0, the max frames check is ignored
//		 * If maxProcessingTime is 0, the max time check is ignored
//		 * maxProcessingTime is specified in seconds
//		 * Calling this function again would continue where it left in the last call
//		 */
//		void Grow(TreeDataPtr tree, int maxFrames = 0, float maxProcessingTime = 0);
//
//		/**
//		 * Reduces the no. of nodes in the tree.  The reduction is controlled
//		 * by [maxTiltAngle].  If tileAngle is 0, then the default tile angle 
//		 * specified in the tree configuration would be used
//		 * tiltAngleDegrees is in Degrees
//		 * The input tree is disposed and a new optimized tree is returned
//		 */
//		TreeDataPtr ReducePoints(TreeDataPtr tree, float tiltAngleDegrees = 0);
//
//		/** Smooths the tree skeleton by applying spline smoothing */
//		TreeDataPtr ApplySplineSmoothing(TreeDataPtr tree);
//
//		/** Build mesh data from the previously generated smooth spline */
//		TreeDataPtr BuildMesh(TreeDataPtr tree);
//
//	private:
//		/** Grow the branch nodes by a frame */
//		void GrowBranchFrame(TreeDataPtr tree);
//
//		void CalculateTreeFrenetFrame(TreeDataPtr tree);
//
//		// Tree branch node visitor functions
//		void _InitializeForNextGrowth(BranchNodePtr node);
//		void _SmoothSkeletalSegment(BranchNodePtr node);
//		void _CalculateNodeFrenetFrame(BranchNodePtr node);
//		void _ResetThickness(BranchNodePtr node);
//		void _UpdateThickness(BranchNodePtr node);
//		void _UpdateLengthFromRoot(BranchNodePtr node);
//		void _GenerateMeshVertices(BranchNodePtr node);
//		void _GenerateMeshLines(BranchNodePtr node);
//		void _GenerateMeshTriangles(BranchNodePtr node);
//
//		/**
//		 * Gets a point on the cubic hermite spline defined by the starting point's 
//		 * position [p0] and tangent [m0] and end point's position [p1] and tangent [m1] 
//		 * http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Unit_interval_.280.2C_1.29
//		 * [t] is from 0 and 1 where 0 is starting point and 1 is end point
//		 */
//		respawn::math::Vector3 GetPointOnSpline(float t, 
//				const respawn::math::Vector3& p0, const respawn::math::Vector3& m0, 
//				const respawn::math::Vector3& p1, const respawn::math::Vector3& m1);
//
//		/** 
//		* Reduces the no. or nodes along its branch.  The reduction is controlled
//		* by [maxTiltAngle].
//		*/
//		BranchNodePtr DecimateNode(BranchNodePtr node, float maxTiltAngle);
//
//		/**
//		 * Reduce the no. of data points on the tree data structure
//		 * Returns an optimized tree with a lot fewer nodes 
//		 */
//		TreeDataPtr DecimateNodes(TreeDataPtr tree, float maxTiltAngle);
//
//		/** Gets the circle radius, given the [area] */
//		float GetRadiusFromArea(float area);
//
//	private:
//		const respawn::EngineContext& engineContext;
//		respawn::system::TimerPtr timer;
//		TreeConfig config;
//
//		int branchNodeVertexIndexCounter;
//		// Base reference for the cross section of a branch
//		Vector3List circlePoints;
//		Vector3List circleNormals;
//	};
//
//	typedef boost::shared_ptr<TreeGenerator> TreeGeneratorPtr;
//
//
//	struct NodeDecimateInfo {
//		NodeDecimateInfo(BranchNodePtr unoptimizedNode, BranchNodePtr optimizedNode) 
//			: unoptimizedNode(unoptimizedNode), optimizedNode(optimizedNode) {}
//		BranchNodePtr unoptimizedNode;
//		BranchNodePtr optimizedNode;
//	};
//}
//
