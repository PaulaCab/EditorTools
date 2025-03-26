// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "ExtendEditor.h"

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAD = InArgs._AssetDataToStore;

	FSlateFontInfo titleFont = GetEmbossedFont();
	titleFont.Size = 20;
	
	ChildSlot
	[
		SNew(SVerticalBox)

		//title
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(titleFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]

		//dropdown
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]

		//asset list
		+SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)

			+SScrollBox::Slot()
			[
				ConstructAssetListView()
			]
		]

		//slot for buttons
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			.Padding(5.f)
			.FillWidth(1.f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center) 
				.Text(FText::FromString(TEXT("Delete All")))
				.OnClicked(this, &SAdvanceDeletionTab::OnDeleteAllButtonClicked)
			]

			+SHorizontalBox::Slot()
			.Padding(5.f)
			.FillWidth(1.f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center) 
				.Text(FText::FromString(TEXT("Select All")))
				.OnClicked(this, &SAdvanceDeletionTab::OnSelectAllButtonClicked)
			]

			+SHorizontalBox::Slot()
			.Padding(5.f)
			.FillWidth(1.f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center) 
				.Text(FText::FromString(TEXT("Deselect All")))
				.OnClicked(this, &SAdvanceDeletionTab::OnDeselectAllButtonClicked)
			]
		]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
	AssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
				.ItemHeight(24.f)
				.ListItemsSource(&StoredAD)
				.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList);

	return AssetListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	if(AssetListView.IsValid())
		AssetListView->RebuildList();
}

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
                                                                const TSharedRef<STableViewBase>& OwnerTable)
{
	if(!AssetDataToDisplay.IsValid())
		return  SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);

	const FString className = AssetDataToDisplay->GetClass()->GetName();
	const FString assetName = AssetDataToDisplay->AssetName.ToString();
	FSlateFontInfo classFont = GetEmbossedFont();
	FSlateFontInfo assetFont = GetEmbossedFont();
	classFont.Size = 10;
	assetFont.Size = 11;
	
	auto listViewRow = SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
	.Padding(FMargin(1.f))
	[
		SNew(SHorizontalBox)
		//check box
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.FillWidth(.05f)
		[
			ConstructCheckBox(AssetDataToDisplay)
		]

		//asset class name
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.FillWidth(.3f)
		[
			ConstructTextForRow(className, classFont)
		]
		
		//asset name display
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		[
			ConstructTextForRow(assetName, assetFont)
		]

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		[
			ConstructButtonForRow(AssetDataToDisplay)
		]
		
	];

	return listViewRow;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> checkBox = SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox)
	.OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
	.Visibility(EVisibility::Visible);

	return checkBox;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRow(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> button = SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SAdvanceDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);
	
	return button;
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRow(const FString& Text, const FSlateFontInfo& FontInfo)
{
	TSharedRef<STextBlock>  textBlock =  SNew(STextBlock)
		.Text(FText::FromString(Text))
		.Font(FontInfo)
		.ColorAndOpacity(FColor::White);

	return textBlock;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState) {
	case ECheckBoxState::Unchecked:
		break;
	case ECheckBoxState::Checked:
		break;
	case ECheckBoxState::Undetermined:
		break;
	}
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData)
{
	auto editorModule = FModuleManager::GetModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));

	if(const bool bDeleted = editorModule.DeleteSingleAsset(*AssetData.Get()))
	{
		if(StoredAD.Contains(AssetData))
			StoredAD.Remove(AssetData);
	}
	
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
	return FReply::Handled();
}
