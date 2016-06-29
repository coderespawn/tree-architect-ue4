//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "SViewportToolBar.h"


/**
* A level viewport toolbar widget that is placed in a viewport
*/
class TREEARCHITECTEDITOR_API STreeThemePreview3DViewportToolbar : public SViewportToolBar
{
public:
	SLATE_BEGIN_ARGS(STreeThemePreview3DViewportToolbar){}
		SLATE_ARGUMENT(TSharedPtr<class STreeThemePreview3DViewport>, Viewport)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> GeneratePropertiesMenu() const;
	TSharedRef<SWidget> GenerateCameraMenu() const;
	TSharedPtr<FExtender> GetViewMenuExtender();

private:
	/** The viewport that we are in */
	TWeakPtr<class STreeThemePreview3DViewport> Viewport;

};


/**
* Class containing commands for level viewport actions
*/
class TREEARCHITECTEDITOR_API FTreeThemeEditorViewportCommands : public TCommands<FTreeThemeEditorViewportCommands> {
public:

	FTreeThemeEditorViewportCommands()
	: TCommands<FTreeThemeEditorViewportCommands>
		(
			TEXT("LAThemeEditorViewport"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "TreeThemeViewport", "Tree Theme Viewport"), // Localized context name for displaying
			NAME_None, //TEXT("EditorViewport"), // Parent context name.  
			FEditorStyle::GetStyleSetName() // Icon Style Set
		)
	{
	}

	TSharedPtr< FUICommandInfo > ShowPropertyTreeTheme;
	TSharedPtr< FUICommandInfo > ShowPropertySkylight;
	TSharedPtr< FUICommandInfo > ShowPropertyDirectionalLight;
	TSharedPtr< FUICommandInfo > ShowPropertyAtmosphericFog;
	TSharedPtr< FUICommandInfo > ToggleDebugData;

public:
	/** Registers our commands with the binding system */
	virtual void RegisterCommands() override;

};