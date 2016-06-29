//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

#include "PreviewScene.h"
#include "SEditorViewport.h"
#include "ITreeThemeEditor.h"
#include "TreeThemeAsset.h"
#include "ThemeEditor/TreeThemeEditor.h"
#include "TreeThemeEditorViewportProperties.h"
#include "GraphEditAction.h"

class ATreeArchitect;

/**
* StaticMesh Editor Preview viewport widget
*/
class TREEARCHITECTEDITOR_API STreeThemePreview3DViewport : public SEditorViewport, public FGCObject, public FTreeThemeEditorViewportPropertiesListener
{
public:
	SLATE_BEGIN_ARGS(STreeThemePreview3DViewport){}
		SLATE_ARGUMENT(TWeakPtr<FTreeThemeEditor>, TreeThemeEditor)
		SLATE_ARGUMENT(UTreeThemeAsset*, ObjectToEdit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~STreeThemePreview3DViewport();

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FGCObject interface

	/** Set the parent tab of the viewport for determining visibility */
	void SetParentTab(TSharedRef<SDockTab> InParentTab) { ParentTab = InParentTab; }

	// SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);
	// End of SWidget Interface

	EVisibility GetToolbarVisibility() const;

	virtual void OnPropertyChanged(FString PropertyName, UTreeThemeEditorViewportProperties* Properties) override;

	void SpawnTreeActor();
	
	ATreeArchitect* GetTree() const { return Tree; }

protected:
	/** SEditorViewport interface */
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual EVisibility OnGetViewportContentVisibility() const override;
	virtual void BindCommands() override;
	virtual void OnFocusViewportToSelection() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;

	void OnShowPropertyTreeTheme();
	void OnShowPropertySkylight();
	void OnShowPropertyDirectionalLight();
	void OnShowPropertyAtmosphericFog();
	void OnToggleDebugData();
	void ShowObjectProperties(UObject* Object);
	
private:
	/** Determines the visibility of the viewport. */
	bool IsVisible() const;


private:
	TWeakPtr<FTreeThemeEditor> LAThemeEditorPtr;
	UTreeThemeAsset* ObjectToEdit;

	/** The parent tab where this viewport resides */
	TWeakPtr<SDockTab> ParentTab;

	/** Level viewport client */
	TSharedPtr<class FTreeThemePreview3DViewportClient> EditorViewportClient;

	/** The scene for this viewport. */
	TSharedPtr<FPreviewScene> PreviewScene;

	/** The landscape actor in the preview 3D viewport */
	ATreeArchitect* Tree;

	USkyLightComponent* Skylight;
	UAtmosphericFogComponent* AtmosphericFog;

};

