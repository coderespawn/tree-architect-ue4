//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "AssetToolsModule.h"
#include "PropertyEditorModule.h"
#include "AssetEditorToolkit.h"
#include "ModuleManager.h"
#include "Asset/TreeThemeAssetTypeAction.h"
#include "ThemeEditor/Preview3D/STreeThemePreview3DViewportToolbar.h"
#include "Customizations/TreeArchitectStyle.h"
#include "Customizations/TreeEditorCustomization.h"

#define LOCTEXT_NAMESPACE "TreeArchitectEditorModule" 


class FTreeArchitectEditorModule : public ITreeArchitectEditorModule
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override {

		FLAStyle::Initialize();

		// Register the details customization
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.RegisterCustomClassLayout("TreeArchitect", FOnGetDetailCustomizationInstance::CreateStatic(&FTreeArchitectCustomization::MakeInstance));
		PropertyEditorModule.NotifyCustomizationModuleChanged();


		// Register asset types
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		RegisterAssetTypeAction(AssetTools, MakeShareable(new FLAThemeAssetTypeAction));

		FTreeThemeEditorViewportCommands::Register();
	}

	virtual void ShutdownModule() override {
		// Unregister all the asset types that we registered
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
			{
				AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
			}
		}
		CreatedAssetTypeActions.Empty();
	}


private:
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		CreatedAssetTypeActions.Add(Action);
	}

	/** All created asset type actions.  Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
};

IMPLEMENT_MODULE(FTreeArchitectEditorModule, TreeArchitectEditorModule)


#undef LOCTEXT_NAMESPACE
