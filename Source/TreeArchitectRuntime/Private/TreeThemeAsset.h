//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "TreeThemeAsset.generated.h"

/** A Dungeon Theme asset lets you design the look and feel of you dungeon with an intuitive graph based approach */
UCLASS(Blueprintable)
class TREEARCHITECTRUNTIME_API UTreeThemeAsset : public UObject {
	GENERATED_UCLASS_BODY()
public:

#if WITH_EDITORONLY_DATA
	class UTreeThemeEditorViewportProperties* PreviewViewportProperties;
#endif // WITH_EDITORONLY_DATA

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
};

