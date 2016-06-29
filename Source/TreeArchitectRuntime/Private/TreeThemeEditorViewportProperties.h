//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

#include "TreeThemeEditorViewportProperties.generated.h"

class FTreeThemeEditorViewportPropertiesListener {
public:
	virtual void OnPropertyChanged(FString PropertyName, class UTreeThemeEditorViewportProperties* Properties) = 0;
};

UCLASS()
class TREEARCHITECTRUNTIME_API UTreeThemeEditorViewportProperties : public UObject {
	GENERATED_BODY()

public:
	UTreeThemeEditorViewportProperties(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	void PostEditChangeConfigProperty(struct FPropertyChangedEvent& e);
#endif // WITH_EDITOR

	TWeakPtr<FTreeThemeEditorViewportPropertiesListener> PropertyChangeListener;
};
