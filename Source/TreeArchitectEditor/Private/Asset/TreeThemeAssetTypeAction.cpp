//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "TreeThemeAssetTypeAction.h"
#include "TreeThemeAsset.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "ThemeEditor/TreeThemeEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FLAThemeAssetTypeAction

FText FLAThemeAssetTypeAction::GetName() const
{
	return LOCTEXT("FLAThemeAssetTypeActionName", "Tree Theme");
}

FColor FLAThemeAssetTypeAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FLAThemeAssetTypeAction::GetSupportedClass() const
{
	return UTreeThemeAsset::StaticClass();
}

void FLAThemeAssetTypeAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UTreeThemeAsset* TreeTheme = Cast<UTreeThemeAsset>(*ObjIt);
		if (TreeTheme) {
			TSharedRef<FTreeThemeEditor> NewTreeEditor(new FTreeThemeEditor);
			NewTreeEditor->InitTreeThemeEditor(Mode, EditWithinLevelEditor, TreeTheme);
		}
	}
}

uint32 FLAThemeAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::Misc | EAssetTypeCategories::Basic;
}

void FLAThemeAssetTypeAction::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
