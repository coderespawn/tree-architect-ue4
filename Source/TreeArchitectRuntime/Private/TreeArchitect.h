//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "TreeThemeAsset.h"
#include "TreeMeshComponent.h"
#include "Algorithm/SpaceColonization/TreeGenerator.h"
#include "TreeArchitect.generated.h"

class UTreeMeshComponent;

UCLASS()
class TREEARCHITECTRUNTIME_API ATreeArchitect : public AActor {
	GENERATED_UCLASS_BODY()

public:
	void Build();


	UPROPERTY(EditAnywhere, Category = Tree)
	UTreeThemeAsset* TreeTheme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	UTreeMeshComponent* TreeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
	FTreeConfig TreeConfig;
};