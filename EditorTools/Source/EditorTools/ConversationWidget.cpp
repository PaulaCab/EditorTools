// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationWidget.h"

#include "AnswerWidget.h"
#include "Conversation.h"
#include "Editor.h"
#include "Components/TextBlock.h"
#include "SpeakerDataAsset.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"


void UConversationWidget::StartConversation(UConversation* NewConversation)
{
	if(!NewConversation || !NewConversation->Lines.Num())
		return;
	
	Conversation = NewConversation;
	CurrentLine = &Conversation->Lines[0];
	ChangeLine(0);
}


void UConversationWidget::ChangeLine(int Index)
{
	if(!Conversation->Lines.IsValidIndex(Index))
		return;

	CurrentLine = &Conversation->Lines[Index];
	CurrentText = CurrentLine->Text.ToString();
	MainTextBlock->SetText(FText::FromString(TEXT("")));
	
	if(auto* world =GetWorld())
		world->GetTimerManager().SetTimer(Text_TH, this, &UConversationWidget::UpdateText, LetterSpeed, true);
	
	UpdatePortrait();
	UpdateAnswers();
}


void UConversationWidget::UpdatePortrait()
{
	if(!SpeakerDA)
		return;
	
	UEmotionDataAsset* emotionDA = SpeakerDA->SpeakerMap[CurrentLine->Speaker];
	if(!emotionDA)
		return;

	NameTextBlock->SetText(emotionDA->CharacterName);
	
	if(UTexture2D* portraitTex = emotionDA->EmotionMap[CurrentLine->Emotion])
		Portrait->SetBrushFromTexture(portraitTex);
}

void UConversationWidget::UpdateAnswers()
{
	if(!AnswerTemplate)
		return;
	
	if(!CurrentLine->bHasAnswer || !CurrentLine->Answers.Num())
	{
		AnswersBorder->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	
	AnswersBorder->SetVisibility(ESlateVisibility::Visible);

	//Create new answer widgets if there's not enough
	if(CurrentLine->Answers.Num()>AnswersArray.Num())
	{
		for(int i = CurrentLine->Answers.Num()-AnswersArray.Num(); i>0; i--)
		{
			UAnswerWidget* newAnswer = CreateWidget<UAnswerWidget>(GetWorld(), AnswerTemplate);
			AnswersBox->AddChild(newAnswer);
			AnswersArray.Add(newAnswer);
		}
	}

	for(int i = 0; i < AnswersArray.Num(); i++)
	{
		if(i<CurrentLine->Answers.Num())
		{
			AnswersArray[i]->SetVisibility(ESlateVisibility::Visible);
			AnswersArray[i]->SetText(CurrentLine->Answers[i].Text);
		}
		else
		{
			AnswersArray[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	AnswersArray[0]->OnSelect();
	AnswerIndex = 0;
}

void UConversationWidget::UpdateText()
{
	if(SkipCount>0)
	{
		SkipCount--;
		return;
	}
	
	StringLenght++;
	MainTextBlock->SetText(FText::FromString(CurrentText.Mid(0, StringLenght)));

	if(CurrentText.Len() == StringLenght)
	{
		if(const auto* world = GetWorld())
			world->GetTimerManager().ClearTimer(Text_TH);
		
		StringLenght = 0;
		return;
	}

	char c = CurrentText.GetCharArray()[StringLenght-1];
	if( c == '!' || c == '.' || c == ',' || c == '?')
	{
		SkipCount = SkipAmount;		
	}
}

void UConversationWidget::PressUp()
{
	if(!Conversation || !CurrentLine || !CurrentLine->bHasAnswer)
		return;

	if(AnswerIndex==0)
		return;

	AnswersArray[AnswerIndex]->OnDiselect();
	AnswerIndex--;
	AnswersArray[AnswerIndex]->OnSelect();
}

void UConversationWidget::PressDown()
{
	if(!Conversation || !CurrentLine || !CurrentLine->bHasAnswer)
		return;
	
	if(CurrentLine->Answers.Num() <= AnswerIndex + 1)
		return;
	
	AnswersArray[AnswerIndex]->OnDiselect();
	AnswerIndex++;
	AnswersArray[AnswerIndex]->OnSelect();
}

void UConversationWidget::PressEnter()
{
	if(!Conversation || !CurrentLine)
		return;

	if(StringLenght!=0)
	{
		MainTextBlock->SetText(FText::FromString(CurrentText));

		if(const auto* world = GetWorld())
			world->GetTimerManager().ClearTimer(Text_TH);
		
		StringLenght = 0;
		return;
	}
	
	if(CurrentLine->bHasAnswer)
	{
		if(!CurrentLine->Answers.IsValidIndex(AnswerIndex))
			return;
	
		AnswersArray[AnswerIndex]->OnDiselect();
		ChangeLine(CurrentLine->Answers[AnswerIndex].NextLine);
	}
	else
	{
		ChangeLine(CurrentLine->NextLine);
	}

}
