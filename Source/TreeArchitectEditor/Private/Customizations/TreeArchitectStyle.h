//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once
#include "EditorStyleSet.h"

/**
* Implements the visual style of Tree Architect plugin
*/
class TREEARCHITECTEDITOR_API FLAStyle : FEditorStyle
{
public:
	static void Initialize();

	static void Shutdown();

	/** @return The Slate style set for CityArchitect Editor */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedRef< class FSlateStyleSet > Create();
	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

	void SetupGeneralStyles();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;

};

