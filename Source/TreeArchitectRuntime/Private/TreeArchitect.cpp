//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeArchitect.h"
#include "TreeThemeAsset.h"
#include "Algorithm/SpaceColonization/TreeGenerator.h"
#include "Algorithm/SpaceColonization/Tree.h"

ATreeArchitect::ATreeArchitect(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) 
{
	USceneComponent* Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "Root");
	RootComponent = Scene;

	TreeMesh = ObjectInitializer.CreateDefaultSubobject<UTreeMeshComponent>(this, "TreeMesh");
	TreeMesh->SetupAttachment(RootComponent);
    //TreeMesh->RegisterComponent();
}

void ATreeArchitect::Build()
{
	// Create the tree and it's generator
	TreeGeneratorPtr treeGenerator = TreeGeneratorPtr(new TreeGenerator);
	TreeDataPtr tree = treeGenerator->Generate(GetWorld(), TreeConfig);
	tree->SetGenerationState(TreeGenerationState::SkeletonGenerationProgress);
    int32 MaxGrowthFrames = tree->GetSpacePoints().Num() * 2;
	treeGenerator->Grow(tree, MaxGrowthFrames, 0);
	tree = treeGenerator->ReducePoints(tree);
	tree = treeGenerator->ApplySplineSmoothing(tree);
	tree = treeGenerator->BuildMesh(tree);

	TArray<TreeDataPtr> TreeLODs;
	TreeLODs.Add(tree);

	TArray<FTreeLODModelInfo> LODData;
	FTreeLODModelInfo LOD0;
	LOD0.LODIndex = 0;
	LOD0.Material = Material;
	LOD0.MinScreenSize = 0;
	LOD0.MaxScreenSize = 1;
	LODData.Add(LOD0);

	TreeMesh->CreateMeshSection(0, TreeLODs, LODData, false);

	//TreeMesh->SetMaterial(0, Material);

	//TreeMesh->ReregisterComponent();
}

