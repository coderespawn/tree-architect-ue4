//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

#include "TreeGenerator.generated.h"

class Tree;
struct FTreeConfig;

class TreeData;
typedef TSharedPtr<TreeData> TreeDataPtr; 

class BranchNode;
typedef TSharedPtr<BranchNode> BranchNodePtr;

USTRUCT(Blueprintable)
struct TREEARCHITECTRUNTIME_API FTreeConfig {
	GENERATED_USTRUCT_BODY()
	FTreeConfig();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	int seed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	int numSpacePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float minSpacePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float attractionPointRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float heightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float innerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float outerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float killDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float smoothingStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float branchTipArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	int circumferenceResolution;

	// The max tilt angle in degrees while performing a data point reduction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float optimizationMaxTilt;

	// The max tilt angle in degrees after performing a spline smoothing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	float smoothMaxTilt;

	float GetOuterDiameter() const;
};

typedef TArray<FVector> Vector3List;

class TREEARCHITECTRUNTIME_API TreeGenerator {
public:
	TreeGenerator();

	/** Generates the branch structure and returns the root node */
	TreeDataPtr Generate(UWorld* World, const FTreeConfig& config);
  
	/**
	* Called when a new branch node is added to the tree
	* This is usually done during the tree growth
	*/
	void OnBranchNodeAdded(TreeDataPtr tree, BranchNodePtr node);

	/** 
		* Grows the tree by maxFrames or till the maxProcessingTime, in seconds, has elapsed 
		* If maxFrames is 0, the max frames check is ignored
		* If maxProcessingTime is 0, the max time check is ignored
		* maxProcessingTime is specified in seconds
		* Calling this function again would continue where it left in the last call
		*/
	void Grow(TreeDataPtr tree, int maxFrames = 0, float maxProcessingTime = 0);

	/**
		* Reduces the no. of nodes in the tree.  The reduction is controlled
		* by [maxTiltAngle].  If tileAngle is 0, then the default tile angle 
		* specified in the tree configuration would be used
		* tiltAngleDegrees is in Degrees
		* The input tree is disposed and a new optimized tree is returned
		*/
	TreeDataPtr ReducePoints(TreeDataPtr tree, float tiltAngleDegrees = 0);

	/** Smooths the tree skeleton by applying spline smoothing */
	TreeDataPtr ApplySplineSmoothing(TreeDataPtr tree);

	/** Build mesh data from the previously generated smooth spline */
	TreeDataPtr BuildMesh(TreeDataPtr tree);

private:
	/** Grow the branch nodes by a frame */
	void GrowBranchFrame(TreeDataPtr tree);

	void CalculateTreeFrenetFrame(TreeDataPtr tree);

	// Tree branch node visitor functions
	void _InitializeForNextGrowth(BranchNodePtr node);
	void _SmoothSkeletalSegment(BranchNodePtr node);
	void _CalculateNodeFrenetFrame(BranchNodePtr node);
	void _ResetThickness(BranchNodePtr node);
	void _UpdateThickness(BranchNodePtr node);
	void _UpdateLengthFromRoot(BranchNodePtr node);
	void _GenerateMeshVertices(BranchNodePtr node);
	void _GenerateMeshLines(BranchNodePtr node);
	void _GenerateMeshTriangles(BranchNodePtr node);

	/**
		* Gets a point on the cubic hermite spline defined by the starting point's 
		* position [p0] and tangent [m0] and end point's position [p1] and tangent [m1] 
		* http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Unit_interval_.280.2C_1.29
		* [t] is from 0 and 1 where 0 is starting point and 1 is end point
		*/
	FVector GetPointOnSpline(float t, 
			const FVector& p0, const FVector& m0, 
			const FVector& p1, const FVector& m1);

	/** 
	* Reduces the no. or nodes along its branch.  The reduction is controlled
	* by [maxTiltAngle].
	*/
	BranchNodePtr DecimateNode(BranchNodePtr node, float maxTiltAngle);

	/**
		* Reduce the no. of data points on the tree data structure
		* Returns an optimized tree with a lot fewer nodes 
		*/
	TreeDataPtr DecimateNodes(TreeDataPtr tree, float maxTiltAngle);

	/** Gets the circle radius, given the [area] */
	float GetRadiusFromArea(float area);

private:
	FTreeConfig config;

	int branchNodeVertexIndexCounter;
	// Base reference for the cross section of a branch
	Vector3List circlePoints;
	Vector3List circleNormals;
};

typedef TSharedPtr<TreeGenerator> TreeGeneratorPtr;


struct NodeDecimateInfo {
	NodeDecimateInfo(BranchNodePtr unoptimizedNode, BranchNodePtr optimizedNode) 
		: unoptimizedNode(unoptimizedNode), optimizedNode(optimizedNode) {}
	BranchNodePtr unoptimizedNode;
	BranchNodePtr optimizedNode;
};
