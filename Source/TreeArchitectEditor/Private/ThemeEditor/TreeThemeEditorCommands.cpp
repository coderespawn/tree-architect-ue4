//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "TreeThemeEditorCommands.h"

#define LOCTEXT_NAMESPACE "FTreeThemeEditorCommands" 

FTreeThemeEditorCommands::FTreeThemeEditorCommands()
	: TCommands<FTreeThemeEditorCommands>("TreeThemeEditor", NSLOCTEXT("Contexts", "TreeThemeEditor", "Tree Theme Editor"), NAME_None, FEditorStyle::GetStyleSetName())
{
}

void FTreeThemeEditorCommands::RegisterCommands()
{
	UI_COMMAND(Rebuild, "Rebuild", "Rebuilds the tree", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE