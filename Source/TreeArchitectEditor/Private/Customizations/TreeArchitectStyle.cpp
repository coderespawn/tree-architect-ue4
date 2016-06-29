//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "TreeArchitectStyle.h"
#include "IPluginManager.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style.RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style.RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style.RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style.RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define TTF_CORE_FONT( RelativePath, ... ) FSlateFontInfo( Style.RootToCoreContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style.RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )
#define OTF_CORE_FONT( RelativePath, ... ) FSlateFontInfo( Style.RootToCoreContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

#define PLUGIN_IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FLAStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )


TSharedPtr< FSlateStyleSet > FLAStyle::StyleInstance = nullptr;

FString FLAStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("TreeArchitect"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

void FLAStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLAStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLAStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("CityArchitectStyle"));
	return StyleSetName;
}

TSharedRef< class FSlateStyleSet > FLAStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet(FLAStyle::GetStyleSetName()));
	StyleRef->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleRef->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	FSlateStyleSet& Style = StyleRef.Get();

	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon48x48(48.0f, 48.0f);


	// Define some 'normal' styles, upon which other variations can be based
	const FSlateFontInfo NormalFont = TTF_CORE_FONT("Fonts/Roboto-Regular", 9);

	FTextBlockStyle NormalText = FTextBlockStyle()
		.SetFont(TTF_CORE_FONT("Fonts/Roboto-Regular", 10))
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f))
		.SetHighlightShape(BOX_BRUSH("Common/TextBlockHighlightShape", FMargin(3.f / 8.f)));


	// Generic styles
	{
		Style.Set("CityArchitect.TabIcon", new PLUGIN_IMAGE_BRUSH("Icons/icon_CityEd_40x", Icon40x40));
		Style.Set("CityArchitect.TabIcon.Small", new PLUGIN_IMAGE_BRUSH("Icons/icon_CityEd_40x", Icon40x40));

		Style.Set("CityEditMode.SetCategoryFlowMap", new IMAGE_BRUSH("Icons/icon_meshpaint_40x", Icon20x20));
		Style.Set("CityEditMode.SetCategoryPopulation", new IMAGE_BRUSH("Icons/icon_ShowSkeletalMeshes_40x", Icon20x20));
		Style.Set("CityEditMode.SetCategoryLandWater", new IMAGE_BRUSH("Icons/icon_Placement_FilterProps_40x", Icon20x20));
		Style.Set("CityEditMode.SetCategoryParkForest", new IMAGE_BRUSH("Icons/icon_Mode_Foliage_40x", Icon20x20));
		Style.Set("CityEditMode.SetCategoryStreetGraph", new IMAGE_BRUSH("Icons/icon_TextureEd_CheckeredBackground_40x", Icon20x20));

	}
	
	Style.Set("CityEditMode.ActiveToolName.Text", FTextBlockStyle(NormalText)
		.SetFont(TTF_CORE_FONT("Fonts/Roboto-Bold", 11))
		.SetShadowOffset(FVector2D(1, 1))
		);

	/*
	// Editor Mode Styles
	{
		Style.Set("CityArchitect.ModePaint", new IMAGE_PLUGIN_BRUSH("Icons/CityEditMode/icon_CityEdMode_Paint_40x", Icon40x40));
		Style.Set("CityArchitect.ModeRectangle", new IMAGE_PLUGIN_BRUSH("Icons/CityEditMode/icon_CityEdMode_Rectangle_40x", Icon40x40));
		Style.Set("CityArchitect.ModeBorder", new IMAGE_PLUGIN_BRUSH("Icons/CityEditMode/icon_CityEdMode_Border_40x", Icon40x40));
		Style.Set("CityArchitect.ModeSelect", new IMAGE_PLUGIN_BRUSH("Icons/CityEditMode/icon_CityEdMode_Select_40x", Icon40x40));
	}
	*/

	return StyleRef;
}



const ISlateStyle& FLAStyle::Get()
{
	return *StyleInstance;
}

