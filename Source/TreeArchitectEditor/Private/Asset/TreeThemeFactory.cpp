//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "TreeThemeFactory.h"
#include "TreeThemeAsset.h"


UTreeThemeFactory::UTreeThemeFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	SupportedClass = UTreeThemeAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

bool UTreeThemeFactory::CanCreateNew() const {
	return true;
}

UObject* UTreeThemeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) {
	UTreeThemeAsset* NewAsset = NewObject<UTreeThemeAsset>(InParent, Class, Name, Flags | RF_Transactional);
	return NewAsset;
}