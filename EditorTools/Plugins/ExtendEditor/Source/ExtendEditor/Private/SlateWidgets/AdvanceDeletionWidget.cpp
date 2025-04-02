// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "DebugHelper.h"
#include "ExtendEditor.h"
#include "ObjectTools.h"

#define LIST_ALL TEXT("List all available assets")
#define LIST_UNUSED TEXT("List unused assets")
#define LIST_SAME_NAME TEXT("List assets with same name")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAD = InArgs._AssetDataToStore;
	DisplayedAD = StoredAD;
	ComboBoxSourceItems.Empty();
	CheckBoxArray.Empty();
	SelectedAD.Empty();

	ComboBoxSourceItems.Add(MakeShared<FString>(LIST_ALL));
	ComboBoxSourceItems.Add(MakeShared<FString>(LIST_UNUSED));
	ComboBoxSourceItems.Add(MakeShared<FString>(LIST_SAME_NAME));
	
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

			+SHorizontalBox::Slot()
			.FillWidth(.2f)
			[
				ConstructComboBox()
			]

			+SHorizontalBox::Slot()
			.FillWidth(.5f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Specify the listing condition in the drop.")))
				.Justification(ETextJustify::Center)
			]

			+SHorizontalBox::Slot()
			.FillWidth(.2f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Current folder:\n" + InArgs._SelectedFolder)))
				.Justification(ETextJustify::Right)
				.AutoWrapText(true)
			]
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
				.ListItemsSource(&DisplayedAD)
				.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList)
				.OnMouseButtonClick(this, &SAdvanceDeletionTab::OnRowMouseButtonClicked);

	return AssetListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	SelectedAD.Empty();
	CheckBoxArray.Empty();
	
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
			ConstructCheckBoxForRow(AssetDataToDisplay)
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

//ROW ELEMENTS
TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBoxForRow(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> checkBox = SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox)
	.OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
	.Visibility(EVisibility::Visible);

	CheckBoxArray.Add(checkBox);
	
	return checkBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState) {
	case ECheckBoxState::Unchecked:
		if(SelectedAD.Contains(AssetData))
			SelectedAD.Remove(AssetData);
		break;
	case ECheckBoxState::Checked:
		SelectedAD.AddUnique(AssetData);
		break;
	case ECheckBoxState::Undetermined:
		break;
	}
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

//COMBO BOX
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> comboBox = SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&ComboBoxSourceItems)
	.OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboContent)
	.OnSelectionChanged(this, &SAdvanceDeletionTab::OnComboSelectionChanged)
	[
		SAssignNew(ComboDisplayTextBlock, STextBlock)
		.Text(FText::FromString(TEXT("List Assets Option")))
	];

	return comboBox;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<STextBlock> textBlock = SNew(STextBlock)
	.Text(FText::FromString(*SourceItem.Get()));

	return textBlock;
}

void SAdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));
	auto editorModule = FModuleManager::GetModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));
	
	if(*SelectedOption.Get() == LIST_ALL)
	{
		DisplayedAD = StoredAD;
	}
	else if(*SelectedOption.Get() == LIST_UNUSED)
	{
		editorModule.ListUnusedAssets(StoredAD, DisplayedAD);
		RefreshAssetListView();
	}
	else if(*SelectedOption.Get() == LIST_SAME_NAME)
	{
		editorModule.ListSameNameAssets(StoredAD, DisplayedAD);
		RefreshAssetListView();
	}
}

void SAdvanceDeletionTab::OnRowMouseButtonClicked(TSharedPtr<FAssetData> AssetData)
{
	auto editorModule = FModuleManager::GetModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));

	editorModule.SyncCBToClickedAsset(AssetData->GetObjectPathString());
}

//BUTTONS FUNCTIONS
FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData)
{
	auto editorModule = FModuleManager::GetModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));

	if(const bool bDeleted = editorModule.DeleteSingleAsset(*AssetData.Get()))
	{
		if(StoredAD.Contains(AssetData))
			StoredAD.Remove(AssetData);

		if(DisplayedAD.Contains(AssetData))
			DisplayedAD.Remove(AssetData);
	}
	
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	if(!SelectedAD.Num())
	{
		ShowMsg(EAppMsgType::Ok, TEXT("No assets currently selected"));
		return FReply::Handled();
	}

	TArray<FAssetData> toDeleteAD;
	for(auto& data : SelectedAD)
	{
		toDeleteAD.Add(*data.Get());
	}

	auto editorModule = FModuleManager::GetModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));
	//deletes data, if succeed refresh list
	if(editorModule.DeleteMultipleAssets(toDeleteAD))
	{
		for(auto& data : SelectedAD)
		{
			if(StoredAD.Contains(data))
			StoredAD.Remove(data);
		}
		RefreshAssetListView();
	}
	
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	if(!CheckBoxArray.Num())
		return FReply::Handled();

	for(const auto& checkBox : CheckBoxArray)
	{
		if(!checkBox->IsChecked())
			checkBox->ToggleCheckedState();
	}
	
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
	if(!CheckBoxArray.Num())
		return FReply::Handled();

	for(const auto& checkBox : CheckBoxArray)
	{
		if(checkBox->IsChecked())
			checkBox->ToggleCheckedState();
	}
	
	return FReply::Handled();
}

