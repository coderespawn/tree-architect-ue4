//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "TreeThemeAsset.h"
#include "TreeArchitect.generated.h"

UCLASS()
class TREEARCHITECTRUNTIME_API ATreeArchitect : public AActor {
	GENERATED_UCLASS_BODY()

public:
	void Build();


	UPROPERTY(EditAnywhere, Category = Tree)
	UTreeThemeAsset* TreeTheme;

};