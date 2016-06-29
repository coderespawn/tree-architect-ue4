//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "STreeThemePreview3DViewportToolbar.h"
#include "STreeThemePreview3DViewport.h"
#include "LevelEditor.h"
#include "SEditorViewportToolBarButton.h"
#include "ISlateMetaData.h"
#include "SEditorViewportToolBarMenu.h"
#include "EditorViewportCommands.h"
#include "SEditorViewportViewMenu.h"


#define LOCTEXT_NAMESPACE "LAThemeEditorViewportToolBar"


void STreeThemePreview3DViewportToolbar::Construct(const FArguments& InArgs)
{
	Viewport = InArgs._Viewport;

	TSharedRef<STreeThemePreview3DViewport> ViewportRef = Viewport.Pin().ToSharedRef();

	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");

	const FMargin ToolbarSlotPadding(2.0f, 2.0f);
	const FMargin ToolbarButtonPadding(2.0f, 0.0f);

	static const FName DefaultForegroundName("DefaultForeground");

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		// Color and opacity is changed based on whether or not the mouse cursor is hovering over the toolbar area
		.ColorAndOpacity(this, &SViewportToolBar::OnGetColorAndOpacity)
		.ForegroundColor(FEditorStyle::GetSlateColor(DefaultForegroundName))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(ToolbarSlotPadding)
				[
					SNew(SEditorViewportToolbarMenu)
					.Label(LOCTEXT("ShowMenuTitle", "Properties"))
					.Cursor(EMouseCursor::Default)
					.ParentToolBar(SharedThis(this))
					.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("EditorViewportToolBar.ShowMenu")))
					.OnGetMenuContent(this, &STreeThemePreview3DViewportToolbar::GeneratePropertiesMenu)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(ToolbarSlotPadding)
				[
					SNew(SEditorViewportToolbarMenu)
					.Label(LOCTEXT("ShowCamMenuTitle", "Camera"))
					.Cursor(EMouseCursor::Default)
					.ParentToolBar(SharedThis(this))
					.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("EditorViewportToolBar.CameraMenu")))
					.OnGetMenuContent(this, &STreeThemePreview3DViewportToolbar::GenerateCameraMenu)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(ToolbarSlotPadding)
				[
					SNew(SEditorViewportViewMenu, ViewportRef, SharedThis(this))
					.Cursor(EMouseCursor::Default)
					.MenuExtenders(GetViewMenuExtender())
					.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewMenuButton")))
				]
			]
		]
	];

	SViewportToolBar::Construct(SViewportToolBar::FArguments());
}


TSharedRef<SWidget> STreeThemePreview3DViewportToolbar::GeneratePropertiesMenu() const
{
	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder PropertiesMenuBuilder(bInShouldCloseWindowAfterMenuSelection, Viewport.Pin()->GetCommandList());

	const FTreeThemeEditorViewportCommands& PreviewViewportActions = FTreeThemeEditorViewportCommands::Get();
	PropertiesMenuBuilder.BeginSection("LevelViewportViewportOptions2");

	PropertiesMenuBuilder.AddMenuEntry(PreviewViewportActions.ShowPropertyTreeTheme);
	PropertiesMenuBuilder.AddMenuEntry(PreviewViewportActions.ShowPropertySkylight);
	PropertiesMenuBuilder.AddMenuEntry(PreviewViewportActions.ShowPropertyDirectionalLight);
	PropertiesMenuBuilder.AddMenuEntry(PreviewViewportActions.ShowPropertyAtmosphericFog);
	PropertiesMenuBuilder.AddMenuEntry(PreviewViewportActions.ToggleDebugData);
	PropertiesMenuBuilder.EndSection();

	return PropertiesMenuBuilder.MakeWidget();
}


TSharedRef<SWidget> STreeThemePreview3DViewportToolbar::GenerateCameraMenu() const
{
	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder CameraMenuBuilder(bInShouldCloseWindowAfterMenuSelection, Viewport.Pin()->GetCommandList());

	// Camera types
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Perspective);

	CameraMenuBuilder.BeginSection("LevelViewportCameraType_Ortho", LOCTEXT("CameraTypeHeader_Ortho", "Orthographic"));
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Top);
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Bottom);
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Left);
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Right);
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Front);
	CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Back);
	CameraMenuBuilder.EndSection();

	return CameraMenuBuilder.MakeWidget();
}

TSharedPtr<FExtender> STreeThemePreview3DViewportToolbar::GetViewMenuExtender()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<FExtender> LevelEditorExtenders = LevelEditorModule.GetMenuExtensibilityManager()->GetAllExtenders();
	return LevelEditorExtenders;
}

void FTreeThemeEditorViewportCommands::RegisterCommands()
{
	UI_COMMAND(ShowPropertyTreeTheme, "TreeTheme", "Shows Tree Theme Properties", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ShowPropertySkylight, "Skylight", "Shows Skylight Properties", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ShowPropertyDirectionalLight, "Directional Light", "Shows Directional Light Properties", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ShowPropertyAtmosphericFog, "Atmospheric Fog", "Shows Atmospheric Fog Properties", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ToggleDebugData, "Toggle Debug Data", "Shows the debug data of the dungeon", EUserInterfaceActionType::Button, FInputGesture());

}

#undef LOCTEXT_NAMESPACE 