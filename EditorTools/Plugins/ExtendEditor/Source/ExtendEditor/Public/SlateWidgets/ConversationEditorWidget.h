// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UConversation;
struct FLine;

struct FLineDisplayData
{
	int32 Index;
	TSharedPtr<FLine> Line;
};

class  SConversationEditorTab : public SCompoundWidget
{

	SLATE_BEGIN_ARGS(SConversationEditorTab){}

	SLATE_ARGUMENT(TArray<UConversation*>, Conversations)
	SLATE_ARGUMENT(FString, SelectedFolder)
	
	SLATE_END_ARGS()


public:
	void Construct(const FArguments& InArgs);

private:
	FText NewName;
	FString FolderPath;
	TArray<TSharedPtr<FString>> SpeakerOptions;
	TArray<TSharedPtr<FString>> EmotionOptions;
	
	TSharedPtr<SListView<TSharedPtr<FLineDisplayData>>> LineListView;
	TSharedPtr<SListView<UConversation*>> ConversationListView;
	TArray<TSharedPtr<FLineDisplayData>> CurrentLineList;
	TArray<UConversation*> ConversationList;
	UConversation* SelectedConversation = nullptr;

	UConversation* CreateConversation();
	FReply OnNewConversationClicked();
	FReply OnDeleteConversationClicked();
	FReply OnSaveConversationClicked();

	TSharedRef<SListView<UConversation*>> ConstructConversationList();
	void OnConvNameClicked(UConversation* Conv);

	void RefreshLines();

	TSharedRef<SListView<TSharedPtr<FLineDisplayData>>> ConstructLineList();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FLineDisplayData> Data, const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructSpeakerComboBox(TSharedPtr<FLine> InLine);
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructEmotionComboBox(TSharedPtr<FLine> InLine);
	TSharedRef<SHorizontalBox> ConstructButtonsHBox(TSharedPtr<FLine> InLine);
	TSharedRef<SWidget> ConstructIconButton(const FSlateBrush* IconBrush, FOnClicked OnClickedCallback);
	
	template<typename TEnum>
	TArray<TSharedPtr<FString>> GetEnumOptions(); 
};


