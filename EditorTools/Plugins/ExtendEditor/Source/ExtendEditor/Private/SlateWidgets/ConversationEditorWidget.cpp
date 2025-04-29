// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/ConversationEditorWidget.h"

#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorTools/Conversation.h"
#include "UObject/SavePackage.h"

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
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					ConstructConversationList()
				]
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
		.Padding(FMargin(10.f, 0.f))
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
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					ConstructLineList()
				]
			]

			+SVerticalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.HAlign(HAlign_Center) 
					.Text(FText::FromString(TEXT("Save Conversation")))
					.OnClicked(this, &SConversationEditorTab::OnSaveConversationClicked)
				]

				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.HAlign(HAlign_Center) 
					.Text(FText::FromString(TEXT("Delete Conversation")))
					.OnClicked(this, &SConversationEditorTab::OnDeleteConversationClicked)
				]
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

FReply SConversationEditorTab::OnSaveConversationClicked()
{
	if(!SelectedConversation)
		return FReply::Handled();

	SelectedConversation->Modify();

	UPackage* Package = SelectedConversation->GetPackage();
	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	const bool bSucceeded = UPackage::SavePackage(Package, nullptr, *PackageFileName, SaveArgs);
	
	// if (Package)
	// {
	// 	TArray<UPackage*> PackagesToSave;
	// 	PackagesToSave.Add(Package);
	//
	// 	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, /*bCheckDirty=*/true, /*bPromptToSave=*/false);
	// }
	if(bSucceeded)
		UE_LOG(LogTemp, Warning, TEXT("Package '%s' was successfully saved"), *PackageName)
	
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
	RefreshLines();
}

void SConversationEditorTab::RefreshLines()
{
	CurrentLineList.Empty();
	int16 index = 0;
	for (FLine& line : SelectedConversation->Lines)
	{
		auto linePtr = TSharedPtr<FLine>(&line, [](FLine*) {});
		CurrentLineList.Add(MakeShared<FLineDisplayData>(FLineDisplayData{index, linePtr}));
		index++;
	}

	if (LineListView.IsValid())
		LineListView->RequestListRefresh();
}

TSharedRef<SListView<TSharedPtr<FLineDisplayData>>> SConversationEditorTab::ConstructLineList()
{
	if(!SelectedConversation)
		return SNew(SListView<TSharedPtr<FLineDisplayData>>);
	
	CurrentLineList.Empty();
	int16 index = 0;
	for (FLine& line : SelectedConversation->Lines)
	{
		auto linePtr = TSharedPtr<FLine>(&line, [](FLine*) {});
		CurrentLineList.Add(MakeShared<FLineDisplayData>(FLineDisplayData{index, linePtr}));
		index++;
	}
	
	LineListView = SNew(SListView<TSharedPtr<FLineDisplayData>>)
	.ItemHeight(24.f)
	.ListItemsSource(&CurrentLineList)
	.OnGenerateRow(this, &SConversationEditorTab::OnGenerateRowForList);
	
	return LineListView.ToSharedRef();
}

TSharedRef<ITableRow> SConversationEditorTab::OnGenerateRowForList(TSharedPtr<FLineDisplayData> Data,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if(!Data || !Data->Line)
		return  SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	
	auto table =  SNew(STableRow<TSharedPtr<FLine>>, OwnerTable)
	.Padding(FMargin(4))
	[
		SNew(SVerticalBox)

		// ─── Index, Speaker & Emotion ────────────────────────
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(0, 5, 10, 0)) 
			[
				SNew(STextBlock)
				.Text(FText::AsNumber(Data->Index))
				.Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
			]
			
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
				ConstructSpeakerComboBox(Data->Line)
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
				ConstructEmotionComboBox(Data->Line)
			]
		]

		// ─── Text (Editable) ─────────────────────────────────
		+ SVerticalBox::Slot()
		.Padding(FMargin(15, 5, 0, 5))
		.AutoHeight()
		[
			SNew(SEditableTextBox)
			.Text(Data->Line->Text)
			.OnTextCommitted_Lambda([Data](const FText& NewText, ETextCommit::Type)
			{
				Data->Line->Text = NewText;
			})
		]

		// ─── HasAnswer + NextLine (condicional) ─────────────
		+ SVerticalBox::Slot()
		.Padding(FMargin(15, 0, 0, 5))
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked(Data->Line->bHasAnswer ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged_Lambda([Data](ECheckBoxState NewState)
				{
					Data->Line->bHasAnswer = (NewState == ECheckBoxState::Checked);
				})
				[
					SNew(STextBlock).Text(FText::FromString("Has Answer"))
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(FMargin(15, 0, 6, 0))
			[
				SNew(SBox)
				.Visibility_Lambda([Data]()
				{
				return Data->Line->bHasAnswer ? EVisibility::Collapsed : EVisibility::Visible;
				})
				[
					SNew(STextBlock).Text(FText::FromString("Next Line:"))
				]
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.Visibility_Lambda([Data]()
				{
					return Data->Line->bHasAnswer ? EVisibility::Collapsed : EVisibility::Visible;
				})
				[
					SNew(SEditableTextBox)
					.Text(FText::AsNumber(Data->Line->NextLine))
					.OnTextCommitted_Lambda([Data](const FText& NewText, ETextCommit::Type)
					{
						Data->Line->NextLine = FCString::Atoi(*NewText.ToString());
					})
				]
			]
		]

		// ─── Answers list (condicional) ──────────────────────
		+ SVerticalBox::Slot()
		.Padding(FMargin(10, 5, 0, 5))
		.AutoHeight()
		[
			//TODO: new slistview
			SNew(SBox)
			.Visibility_Lambda([Data]()
			{
				return Data->Line->bHasAnswer ? EVisibility::Visible : EVisibility::Collapsed;
			})
			[
				// Acá podrías poner un SListView o simplemente un scroll editable de respuestas
				SNew(STextBlock).Text(FText::FromString("Answers UI aquí")) // Placeholder
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 4, 0, 0)
		[
			ConstructButtonsHBox(Data->Line)
		]
	];
	
	return table;
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
}

TSharedRef<SHorizontalBox> SConversationEditorTab::ConstructButtonsHBox(TSharedPtr<FLine> InLine)
{
	auto horizontalBox = SNew(SHorizontalBox)

	+ SHorizontalBox::Slot()
	.FillWidth(1.f)
	[
		SNew(SSpacer) // Para empujar los botones a la derecha
	]

	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
			ConstructIconButton(FCoreStyle::Get().GetBrush("Icons.ChevronUp"),
			FOnClicked::CreateLambda([InLine]() {
			// Mover hacia arriba
			return FReply::Handled();
		}))
	]

	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		ConstructIconButton(FCoreStyle::Get().GetBrush("Icons.ChevronDown"),
		FOnClicked::CreateLambda([InLine]() {
			// Mover hacia arriba
			return FReply::Handled();
		}))
	]

	// Add line
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		ConstructIconButton(FAppStyle::Get().GetBrush("Icons.Plus"),
		FOnClicked::CreateLambda([InLine,this]() {
			if(SelectedConversation)
			{
				SelectedConversation->NewLine(*InLine);
				RefreshLines();
			}

			return FReply::Handled();
		}))
	]

	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		ConstructIconButton(FAppStyle::Get().GetBrush("Icons.Delete"),
		FOnClicked::CreateLambda([InLine,this]() {
			if(SelectedConversation)
			{
				SelectedConversation->DeleteLine(*InLine);
				RefreshLines();
			}
			return FReply::Handled();
		}))
	];

	return horizontalBox;
}

TSharedRef<SWidget> SConversationEditorTab::ConstructIconButton(const FSlateBrush* IconBrush, FOnClicked OnClickedCallback)
{
	auto button =  SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "SimpleButton")
		.ContentPadding(2)
		.OnClicked(OnClickedCallback)
		[
			SNew(SImage)
			.Image(IconBrush)
			.ColorAndOpacity(FSlateColor::UseForeground())
		];
	
	return button;
	
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


