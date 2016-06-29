//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "STreeThemePreview3DViewport.h"
#include "EditorViewportClient.h"
#include "TreeThemePreview3DViewportClient.h"
#include "SDockTab.h"
#include "STreeThemePreview3DViewportToolbar.h"
#include "TreeArchitect.h"

#define LOCTEXT_NAMESPACE "STreeThemePreview3DViewport"


void STreeThemePreview3DViewport::Construct(const FArguments& InArgs)
{
	LAThemeEditorPtr = InArgs._TreeThemeEditor;
	ObjectToEdit = InArgs._ObjectToEdit;

	PreviewScene = MakeShareable(new FPreviewScene);
	SEditorViewport::Construct(SEditorViewport::FArguments());
	//ObjectToEdit->PreviewViewportProperties->PropertyChangeListener = SharedThis(this);

	UWorld* World = PreviewScene->GetWorld();

	Skylight = NewObject<USkyLightComponent>();
	PreviewScene->AddComponent(Skylight, FTransform::Identity);
	
	AtmosphericFog = NewObject<UAtmosphericFogComponent>();
	PreviewScene->AddComponent(AtmosphericFog, FTransform::Identity);

	PreviewScene->DirectionalLight->SetMobility(EComponentMobility::Movable);
	PreviewScene->DirectionalLight->CastShadows = true;
	PreviewScene->DirectionalLight->CastStaticShadows = true;
	PreviewScene->DirectionalLight->CastDynamicShadows = true;

	SpawnTreeActor();
}



STreeThemePreview3DViewport::~STreeThemePreview3DViewport()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
	if (EditorViewportClient.IsValid())
	{
		EditorViewportClient->Viewport = NULL;
	}
}

void STreeThemePreview3DViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ObjectToEdit);
}


void STreeThemePreview3DViewport::OnShowPropertyTreeTheme()
{
	ShowObjectProperties(ObjectToEdit->PreviewViewportProperties);
}

void STreeThemePreview3DViewport::OnShowPropertySkylight()
{
	ShowObjectProperties(Skylight);
}

void STreeThemePreview3DViewport::OnShowPropertyDirectionalLight()
{
	ShowObjectProperties(PreviewScene->DirectionalLight);
}

void STreeThemePreview3DViewport::OnShowPropertyAtmosphericFog()
{
	ShowObjectProperties(AtmosphericFog);
}

void STreeThemePreview3DViewport::OnToggleDebugData()
{

}

void STreeThemePreview3DViewport::ShowObjectProperties(UObject* Object)
{
	if (LAThemeEditorPtr.IsValid()) {
		LAThemeEditorPtr.Pin()->ShowObjectDetails(Object);
	}
}

void STreeThemePreview3DViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

}

EVisibility STreeThemePreview3DViewport::GetToolbarVisibility() const
{
	return EVisibility::Visible;
}

TSharedRef<FEditorViewportClient> STreeThemePreview3DViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FTreeThemePreview3DViewportClient(LAThemeEditorPtr, SharedThis(this), *PreviewScene, ObjectToEdit));

	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetRealtime(true);	// TODO: Check if real-time is needed
	EditorViewportClient->VisibilityDelegate.BindSP(this, &STreeThemePreview3DViewport::IsVisible);

	return EditorViewportClient.ToSharedRef();
}

EVisibility STreeThemePreview3DViewport::OnGetViewportContentVisibility() const
{
	return EVisibility::Visible;
}

void STreeThemePreview3DViewport::BindCommands()
{
	SEditorViewport::BindCommands();

	const FTreeThemeEditorViewportCommands& ViewportActions = FTreeThemeEditorViewportCommands::Get();
	CommandList->MapAction(
		ViewportActions.ShowPropertyTreeTheme,
		FExecuteAction::CreateSP(this, &STreeThemePreview3DViewport::OnShowPropertyTreeTheme));

	CommandList->MapAction(
		ViewportActions.ShowPropertySkylight,
		FExecuteAction::CreateSP(this, &STreeThemePreview3DViewport::OnShowPropertySkylight));

	CommandList->MapAction(
		ViewportActions.ShowPropertyDirectionalLight,
		FExecuteAction::CreateSP(this, &STreeThemePreview3DViewport::OnShowPropertyDirectionalLight));

	CommandList->MapAction(
		ViewportActions.ShowPropertyAtmosphericFog,
		FExecuteAction::CreateSP(this, &STreeThemePreview3DViewport::OnShowPropertyAtmosphericFog));

	CommandList->MapAction(
		ViewportActions.ToggleDebugData,
		FExecuteAction::CreateSP(this, &STreeThemePreview3DViewport::OnToggleDebugData));

}

void STreeThemePreview3DViewport::OnFocusViewportToSelection()
{
	SEditorViewport::OnFocusViewportToSelection();
}

TSharedPtr<SWidget> STreeThemePreview3DViewport::MakeViewportToolbar()
{
	// Build our toolbar level toolbar
	TSharedRef< STreeThemePreview3DViewportToolbar > ToolBar =
		SNew(STreeThemePreview3DViewportToolbar)
		.Viewport(SharedThis(this))
		.Visibility(this, &STreeThemePreview3DViewport::GetToolbarVisibility)
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute());

	return
		SNew(SVerticalBox)
		.Visibility(EVisibility::SelfHitTestInvisible)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		[
			ToolBar
		];
}

bool STreeThemePreview3DViewport::IsVisible() const
{
	return ViewportWidget.IsValid() && (!ParentTab.IsValid() || ParentTab.Pin()->IsForeground());
}


void STreeThemePreview3DViewport::OnPropertyChanged(FString PropertyName, UTreeThemeEditorViewportProperties* Properties)
{

}

void STreeThemePreview3DViewport::SpawnTreeActor()
{
	UWorld* World = PreviewScene->GetWorld();
	Tree = World->SpawnActor<ATreeArchitect>(FVector::ZeroVector, FRotator());
}


#undef LOCTEXT_NAMESPACE
