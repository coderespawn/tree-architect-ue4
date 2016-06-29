//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeThemeAsset.h"

UTreeThemeAsset::UTreeThemeAsset(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), PreviewViewportProperties(nullptr) {
}

void UTreeThemeAsset::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UTreeThemeAsset* This = CastChecked<UTreeThemeAsset>(InThis);

	Super::AddReferencedObjects(This, Collector);
}
