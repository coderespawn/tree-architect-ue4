//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once
#include "STreeThemePreview3DViewport.h"
#include "ITreeThemeEditor.h"
#include "PreviewScene.h"

//FTreeThemePreview3DViewportClient

/** Viewport Client for the preview viewport */
class TREEARCHITECTEDITOR_API FTreeThemePreview3DViewportClient : public FEditorViewportClient, public TSharedFromThis<FTreeThemePreview3DViewportClient>
{
public:
	FTreeThemePreview3DViewportClient(TWeakPtr<ITreeThemeEditor> InLAThemeEditor, TWeakPtr<STreeThemePreview3DViewport> InLAThemeEditorViewport, FPreviewScene& InPreviewScene, UTreeThemeAsset* InProp);

	// FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	// End of FEditorViewportClient interface
private:
	TWeakPtr<ITreeThemeEditor> InLAThemeEditor;
	TWeakPtr<STreeThemePreview3DViewport> InLAThemeEditorViewport;
	UTreeThemeAsset* InProp;
};

