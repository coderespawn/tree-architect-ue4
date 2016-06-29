//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeThemeEditorViewportProperties.h"


UTreeThemeEditorViewportProperties::UTreeThemeEditorViewportProperties(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UTreeThemeEditorViewportProperties::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	if (PropertyChangeListener.IsValid()) {
		PropertyChangeListener.Pin()->OnPropertyChanged(e.Property->GetName(), this);
	}
}

void UTreeThemeEditorViewportProperties::PostEditChangeConfigProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyChangeListener.IsValid()) {
		PropertyChangeListener.Pin()->OnPropertyChanged(e.Property->GetName(), this);
	}
}
#endif // WITH_EDITOR
