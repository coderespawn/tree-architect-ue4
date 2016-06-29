//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectEditorPrivatePCH.h"
#include "TreeEditorCustomization.h"
#include "PropertyEditing.h"
#include "SNotificationList.h"
#include "NotificationManager.h"
#include "ContentBrowserModule.h"
#include "IDetailsView.h"
#include "ObjectTools.h"
#include "TreeArchitect.h"
#include "LevelEditor.h"
#include "ILevelViewport.h"
#include "EditorViewportClient.h"


#define LOCTEXT_NAMESPACE "TreeArchitectEditorModule" 

void ShowNotification(FText Text, SNotificationItem::ECompletionState State = SNotificationItem::CS_Fail) {
	FNotificationInfo Info(Text);
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 1.0f;
	Info.ExpireDuration = 2.0f;

	TWeakPtr<SNotificationItem> NotificationPtr = FSlateNotificationManager::Get().AddNotification(Info);
	if (NotificationPtr.IsValid())
	{
		NotificationPtr.Pin()->SetCompletionState(State);
	}
}

void SwitchToRealtimeMode() {
	FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	if (client) {
		bool bRealtime = client->IsRealtime();
		if (!bRealtime) {
			ShowNotification(NSLOCTEXT("TreeRealtimeMode", "TreeRealtimeMode", "Switched viewport to Realtime mode"), SNotificationItem::CS_None);
			client->SetRealtime(true);
		}
	}
	else {
		ShowNotification(NSLOCTEXT("ClientNotFound", "ClientNotFound", "Warning: Cannot find active viewport"));
	}

}


template<typename T>
T* GetActor(IDetailLayoutBuilder* DetailBuilder) {
	TArray<TWeakObjectPtr<UObject>> OutObjects;
	DetailBuilder->GetObjectsBeingCustomized(OutObjects);
	if (OutObjects.Num() > 0) {
		return Cast<T>(OutObjects[0].Get());
	}
	return nullptr;
}

FVector GetLevelViewportLocation() {
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr< ILevelViewport > ViewportWindow = LevelEditorModule.GetFirstActiveViewport();
	if (ViewportWindow.IsValid())
	{
		FEditorViewportClient &Viewport = ViewportWindow->GetLevelViewportClient();
		return Viewport.GetViewLocation();
	}
	return FVector::ZeroVector;
}


void FTreeArchitectCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Tree");

	Category.AddCustomRow(LOCTEXT("TreeArchitectCommand_FilterBuildTree", "build tree"))
		.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("TreeCommand_BuildTree", "Build Tree"))
			.OnClicked(FOnClicked::CreateStatic(&FTreeArchitectCustomization::BuildTree, &DetailBuilder))
		];
}

TSharedRef<IDetailCustomization> FTreeArchitectCustomization::MakeInstance()
{
	return MakeShareable(new FTreeArchitectCustomization);
}

FReply FTreeArchitectCustomization::BuildTree(IDetailLayoutBuilder* DetailBuilder)
{
	ATreeArchitect* Tree = GetActor<ATreeArchitect>(DetailBuilder);
	if (Tree) {
		//Tree->SetBuildLocation(GetLevelViewportLocation());
		Tree->Build();
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE