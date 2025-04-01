// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"



class SAdvanceDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvanceDeletionTab){}

	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDataToStore)
	
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TArray<TSharedPtr<FAssetData>> StoredAD;
	TArray<TSharedPtr<FAssetData>> DisplayedAD;
	TArray<TSharedPtr<FAssetData>> SelectedAD;
	TArray<TSharedPtr<SCheckBox>> CheckBoxArray;
	TArray<TSharedPtr<FString>> ComboBoxSourceItems;

	TSharedPtr<STextBlock> ComboDisplayTextBlock;
	
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;
	void RefreshAssetListView();
	
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> SourceItem); 
	
	TSharedRef<SCheckBox> ConstructCheckBoxForRow(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedRef<SButton> ConstructButtonForRow(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedRef<STextBlock> ConstructTextForRow(const FString& Text, const FSlateFontInfo& FontInfo);
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData);
	FReply OnDeleteAllButtonClicked();
	FReply OnSelectAllButtonClicked();
	FReply OnDeselectAllButtonClicked();
	
	static FSlateFontInfo GetEmbossedFont() {return FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));}

	
};