// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UConversation;
struct FLine;

class  SConversationEditorTab : public SCompoundWidget
{

	SLATE_BEGIN_ARGS(SConversationEditorTab){}

	SLATE_ARGUMENT(TArray<UConversation*>, Conversations)
	SLATE_ARGUMENT(FString, SelectedFolder)
	
	SLATE_END_ARGS()


public:
	void Construct(const FArguments& InArgs);

private:
	FString FolderPath;
	TArray<TSharedPtr<FString>> SpeakerOptions;
	TArray<TSharedPtr<FString>> EmotionOptions;
	
	TSharedPtr<SListView<TSharedPtr<FLine>>> LineListView;
	TArray<TSharedPtr<FLine>> CurrentLineList;
	TArray<UConversation*> ConversationList;
	UConversation* SelectedConversation = nullptr;
	
	FReply OnNewConversationClicked();
	FReply OnDeleteConversationClicked();

	TSharedRef<SListView<UConversation*>> ConstructConversationList();
	void OnConvNameClicked(UConversation* Conv);

	TSharedRef<SListView<TSharedPtr<FLine>>> ConstructLineList();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FLine> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructSpeakerComboBox(TSharedPtr<FLine> InLine);
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructEmotionComboBox(TSharedPtr<FLine> InLine);
	
	template<typename TEnum>
	TArray<TSharedPtr<FString>> GetEnumOptions(); 
};


