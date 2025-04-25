// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/ConversationEditorWidget.h"
#include "EditorTools/Conversation.h"

void SConversationEditorTab::Construct(const FArguments& InArgs)
{
	FolderPath = InArgs._SelectedFolder;
	ConversationList = InArgs._Conversations;
	if(ConversationList.Num())
		SelectedConversation = ConversationList[0];
	else
		return;
	
	SpeakerOptions = GetEnumOptions<ESpeaker>();
	EmotionOptions = GetEnumOptions<EEmotion>();
	
	FSlateFontInfo titleFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	titleFont.Size = 14;
	
	ChildSlot
	[
		SNew(SHorizontalBox)

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SVerticalBox)
			
			+SVerticalBox::Slot()
			.Padding(FMargin(0.f, 10.f, 0.f, 0.f))
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Conversations")))
				.Font(titleFont)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FColor::White)
			]
			
			+SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			[
				ConstructConversationList()
			]

			+SVerticalBox::Slot()
			.AutoHeight()
			[
			SNew(SButton)
			.HAlign(HAlign_Center) 
			.Text(FText::FromString(TEXT("New Conversation")))
			.OnClicked(this, &SConversationEditorTab::OnNewConversationClicked)
			]
		]

		+SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(SVerticalBox)
				
			+SVerticalBox::Slot()
			.Padding(FMargin(0.f, 10.f, 0.f, 0.f))
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(SelectedConversation ? SelectedConversation->ConversationName : TEXT("Invalid")))
				.Font(titleFont)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FColor::White)
			]

			+SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			[
				ConstructLineList()
			]
			
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.HAlign(HAlign_Center) 
				.Text(FText::FromString(TEXT("Delete Conversation")))
				.OnClicked(this, &SConversationEditorTab::OnDeleteConversationClicked)
			]
		]
	];
}


FReply SConversationEditorTab::OnNewConversationClicked()
{
	//TODO:
	return FReply::Handled();
}

FReply SConversationEditorTab::OnDeleteConversationClicked()
{
	//TODO:
	return FReply::Handled();
}


TSharedRef<SListView<UConversation*>> SConversationEditorTab::ConstructConversationList()
{
	auto convList = SNew(SListView<UConversation*>)
	.ItemHeight(24.f)
	.ListItemsSource(&ConversationList)
	.OnGenerateRow_Lambda([](UConversation* InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		return SNew(STableRow<UConversation*>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(IsValid(InItem) ? InItem->ConversationName : TEXT("Invalid Item")))
		];
	})
	.OnMouseButtonClick(this, &SConversationEditorTab::OnConvNameClicked);

	return convList;
}



void SConversationEditorTab::OnConvNameClicked(UConversation* Conv)
{
	if(!Conv)
		return;
	
	SelectedConversation = Conv;

	CurrentLineList.Empty();
	for (FLine& line : SelectedConversation->Lines)
	{
		CurrentLineList.Add(TSharedPtr<FLine>(&line, [](FLine*) {}));
	}

	if (LineListView.IsValid())
		LineListView->RequestListRefresh();
}

TSharedRef<SListView<TSharedPtr<FLine>>> SConversationEditorTab::ConstructLineList()
{
	if(!SelectedConversation)
		return SNew(SListView<TSharedPtr<FLine>>);
	
	CurrentLineList.Empty();
	for (FLine& line : SelectedConversation->Lines)
	{
		CurrentLineList.Add(TSharedPtr<FLine>(&line, [](FLine*) {}));
	}
	
	LineListView = SNew(SListView<TSharedPtr<FLine>>)
	.ItemHeight(24.f)
	.ListItemsSource(&CurrentLineList)
	.OnGenerateRow(this, &SConversationEditorTab::OnGenerateRowForList);
	
	return LineListView.ToSharedRef();
}

TSharedRef<ITableRow> SConversationEditorTab::OnGenerateRowForList(TSharedPtr<FLine> InLine,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if(!InLine)
		return  SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	
	return SNew(STableRow<TSharedPtr<FLine>>, OwnerTable)
	.Padding(FMargin(4))
	[
		SNew(SVerticalBox)

		// ─── Speaker & Emotion ─────────────────────────────────
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(0, 0, 6, 0))
			[
				SNew(STextBlock).Text(FText::FromString("Speaker:"))
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				ConstructSpeakerComboBox(InLine)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(10, 0, 6, 0))
			[
				SNew(STextBlock).Text(FText::FromString("Emotion:"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				ConstructEmotionComboBox(InLine)
			]
		]

		// ─── Text (Editable) ─────────────────────────────────
		+ SVerticalBox::Slot()
		.Padding(FMargin(0, 5, 0, 5))
		.AutoHeight()
		[
			SNew(SEditableTextBox)
			.Text(InLine->Text)
			.OnTextCommitted_Lambda([InLine](const FText& NewText, ETextCommit::Type)
			{
				InLine->Text = NewText;
			})
		]

		// ─── HasAnswer + NextLine (condicional) ─────────────
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked(InLine->bHasAnswer ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged_Lambda([InLine](ECheckBoxState NewState)
				{
					InLine->bHasAnswer = (NewState == ECheckBoxState::Checked);
				})
				[
					SNew(STextBlock).Text(FText::FromString("Has Answer"))
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(FMargin(10, 0, 6, 0))
			[
				SNew(SBox)
				.Visibility_Lambda([InLine]()
				{
				return InLine->bHasAnswer ? EVisibility::Collapsed : EVisibility::Visible;
				})
				[
					SNew(STextBlock).Text(FText::FromString("Next Line:"))
				]
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.Visibility_Lambda([InLine]()
				{
					return InLine->bHasAnswer ? EVisibility::Collapsed : EVisibility::Visible;
				})
				[
					SNew(SEditableTextBox)
					.Text(FText::AsNumber(InLine->NextLine))
					.OnTextCommitted_Lambda([InLine](const FText& NewText, ETextCommit::Type)
					{
						InLine->NextLine = FCString::Atoi(*NewText.ToString());
					})
				]
			]
		]

		// ─── Answers list (condicional) ──────────────────────
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			//TODO: new slistview
			SNew(SBox)
			.Visibility_Lambda([InLine]()
			{
				return InLine->bHasAnswer ? EVisibility::Visible : EVisibility::Collapsed;
			})
			[
				// Acá podrías poner un SListView o simplemente un scroll editable de respuestas
				SNew(STextBlock).Text(FText::FromString("Answers UI aquí")) // Placeholder
			]
		]
	];
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SConversationEditorTab::ConstructSpeakerComboBox(TSharedPtr<FLine> InLine)
{
	auto comboBox = SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&SpeakerOptions)
	.InitiallySelectedItem(SpeakerOptions[(int32)InLine->Speaker])
	.OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem)
	{
		return SNew(STextBlock).Text(FText::FromString(*InItem));
	})
	.OnSelectionChanged_Lambda([InLine](TSharedPtr<FString> NewSelection, ESelectInfo::Type)
	{
		int32 Index = StaticEnum<ESpeaker>()->GetIndexByNameString(*NewSelection);
		if (Index != INDEX_NONE)
		{
			InLine->Speaker = static_cast<ESpeaker>(StaticEnum<ESpeaker>()->GetValueByIndex(Index));
		}
	})
	[
		SNew(STextBlock).Text_Lambda([InLine, this]()
		{
			if (SpeakerOptions.IsValidIndex((int32)InLine->Speaker))
			{
				return FText::FromString(*SpeakerOptions[(int32)InLine->Speaker]);
			}
			return FText::FromString("Invalid");
		})
	];
	
	return comboBox;
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SConversationEditorTab::ConstructEmotionComboBox(TSharedPtr<FLine> InLine)
{
	auto comboBox = SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&EmotionOptions)
	.InitiallySelectedItem(EmotionOptions[(int32)InLine->Emotion])
	.OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem)
	{
		return SNew(STextBlock).Text(FText::FromString(*InItem));
	})
	.OnSelectionChanged_Lambda([InLine](TSharedPtr<FString> NewSelection, ESelectInfo::Type)
	{
		int32 Index = StaticEnum<EEmotion>()->GetIndexByNameString(*NewSelection);
		if (Index != INDEX_NONE)
		{
			InLine->Emotion = static_cast<EEmotion>(StaticEnum<EEmotion>()->GetValueByIndex(Index));
		}
	})
	[
	SNew(STextBlock).Text_Lambda([InLine, this]()
	{
		if (EmotionOptions.IsValidIndex((int32)InLine->Emotion))
		{
			return FText::FromString(*EmotionOptions[(int32)InLine->Emotion]);
		}
		return FText::FromString("Invalid");
	})
];

	return comboBox;
};

template <typename TEnum>
TArray<TSharedPtr<FString>> SConversationEditorTab::GetEnumOptions()
{
	TArray<TSharedPtr<FString>> Options;
	UEnum* EnumPtr = StaticEnum<TEnum>();
	if (!EnumPtr) return Options;

	for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
	{
		Options.Add(MakeShared<FString>(EnumPtr->GetNameStringByIndex(i)));
	}
	return Options;

	
}


