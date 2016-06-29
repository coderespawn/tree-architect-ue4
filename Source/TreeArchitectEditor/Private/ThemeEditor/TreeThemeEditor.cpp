//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "Toolkits/IToolkitHost.h"
#include "TreeThemeEditor.h"
#include "SNodePanel.h"
#include "SDockTab.h"
#include "SSingleObjectDetailsPanel.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "EditorSupportDelegates.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Preview3D/STreeThemePreview3DViewport.h"
#include "TreeThemeEditorCommands.h"
#include "TreeArchitect.h"


#define LOCTEXT_NAMESPACE "FTreeThemeEditor" 

DEFINE_LOG_CATEGORY_STATIC(TreeThemeEditor, Log, All);

const FName LAThemeEditorAppName = FName(TEXT("LAThemeEditorApp"));

struct FTreeThemeEditorTabs
{
	// Tab identifiers
	static const FName Preview3DID;
	static const FName DetailsID;
};


//////////////////////////////////////////////////////////////////////////

const FName FTreeThemeEditorTabs::DetailsID(TEXT("Details"));
const FName FTreeThemeEditorTabs::Preview3DID(TEXT("Preview3D"));

FName FTreeThemeEditor::GetToolkitFName() const
{
	return FName("TreeThemeEditor");
}

FText FTreeThemeEditor::GetBaseToolkitName() const
{
	return LOCTEXT("LAThemeEditorAppLabel", "Tree Theme Editor");
}

FText FTreeThemeEditor::GetToolkitName() const
{
	const bool bDirtyState = ThemeBeingEdited->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("ThemeName"), FText::FromString(ThemeBeingEdited->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("TreeThemeEditorAppLabel", "{ThemeName}{DirtyState}"), Args);
}

FString FTreeThemeEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("TreeArchitectThemeEditor");
}

FString FTreeThemeEditor::GetDocumentationLink() const
{
	return TEXT("TreeArchitect/ThemeEditor");
}

void FTreeThemeEditor::NotifyPreChange(UProperty* PropertyAboutToChange)
{

}

void FTreeThemeEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{

}

FLinearColor FTreeThemeEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

//////////////////////////////////////////////////////////////////////////
// FTreeThemeEditor

TSharedRef<SDockTab> FTreeThemeEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;

	// Spawn the tab
	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
		[
			PropertyEditorRef
		];
}

bool FTreeThemeEditor::IsTickableInEditor() const
{
	return true;
}

void FTreeThemeEditor::Tick(float DeltaTime)
{
}

bool FTreeThemeEditor::IsTickable() const
{
	return true;
}

TStatId FTreeThemeEditor::GetStatId() const
{
	return TStatId();
}

TSharedRef<SDockTab> FTreeThemeEditor::SpawnTab_Preview3D(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> SpawnedTab =
		SNew(SDockTab)
		.Label(LOCTEXT("Preview3D", "Preview 3D"))
		.TabColorScale(GetTabColorScale())
		[
			PreviewViewport.ToSharedRef()
		];

	PreviewViewport->SetParentTab(SpawnedTab);
	return SpawnedTab;
}

void FTreeThemeEditor::SaveAsset_Execute()
{
	UE_LOG(TreeThemeEditor, Log, TEXT("Saving landscape theme %s"), *GetEditingObjects()[0]->GetName());

	UpdateOriginalThemeAsset();

	UPackage* Package = ThemeBeingEdited->GetOutermost();
	if (Package)
	{
		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(Package);

		FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
	}

}

void FTreeThemeEditor::UpdateOriginalThemeAsset()
{
	FEditorDelegates::RefreshEditor.Broadcast();
	FEditorSupportDelegates::RedrawAllViewports.Broadcast();
}

FTreeThemeEditor::~FTreeThemeEditor()
{

}

void FTreeThemeEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_TreeThemeEditor", "Tree Theme Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(FTreeThemeEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FTreeThemeEditor::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTabLabel", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	TabManager->RegisterTabSpawner(FTreeThemeEditorTabs::Preview3DID, FOnSpawnTab::CreateSP(this, &FTreeThemeEditor::SpawnTab_Preview3D))
		.SetDisplayName(LOCTEXT("Preview3DTab", "Preview 3D"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
}

void FTreeThemeEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(FTreeThemeEditorTabs::Preview3DID);
	TabManager->UnregisterTabSpawner(FTreeThemeEditorTabs::DetailsID);
}

void FTreeThemeEditor::ExtendMenu() {
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Tree");
			{
				ToolbarBuilder.AddToolBarButton(FTreeThemeEditorCommands::Get().Rebuild);
			}
			ToolbarBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar)
		);
	AddToolbarExtender(ToolbarExtender);
}


void FTreeThemeEditor::BindCommands()
{

	const FTreeThemeEditorCommands& Commands = FTreeThemeEditorCommands::Get();

	ToolkitCommands->MapAction(
		Commands.Rebuild,
		FExecuteAction::CreateSP(this, &FTreeThemeEditor::HandleRebuildActionExecute));
}

void FTreeThemeEditor::HandleRebuildActionExecute()
{
	RebuildTree();
}

void FTreeThemeEditor::RebuildTree()
{
	ATreeArchitect* Tree = PreviewViewport->GetTree();
	Tree->Build();
}

void FTreeThemeEditor::InitTreeThemeEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UTreeThemeAsset* TreeTheme) {

	// Initialize the asset editor and spawn nothing (dummy layout)
	FAssetEditorManager::Get().CloseOtherEditors(TreeTheme, this);
	ThemeBeingEdited = TreeTheme;

	FTreeThemeEditorCommands::Register();
	BindCommands();
	ExtendMenu();

	PreviewViewport = SNew(STreeThemePreview3DViewport)
		.TreeThemeEditor(SharedThis(this))
		.ObjectToEdit(ThemeBeingEdited);


	// Default layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_TreeArchitectEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
				)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(1.0f)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(FTreeThemeEditorTabs::DetailsID, ETabState::OpenedTab)
				)

				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->SetHideTabWell(true)
					->AddTab(FTreeThemeEditorTabs::Preview3DID, ETabState::OpenedTab)
				)
			)
		);

	// Initialize the asset editor and spawn nothing (dummy layout)
	InitAssetEditor(Mode, InitToolkitHost, LAThemeEditorAppName, StandaloneDefaultLayout, /*bCreateDefaultStandaloneMenu=*/ true, /*bCreateDefaultToolbar=*/ true, TreeTheme);
	
}

void FTreeThemeEditor::ShowObjectDetails(UObject* ObjectProperties)
{
	PropertyEditor->SetObject(ObjectProperties);
}

#undef LOCTEXT_NAMESPACE

