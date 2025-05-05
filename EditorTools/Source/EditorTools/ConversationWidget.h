// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConversationWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UImage;
class UBorder;
class UAnswerWidget;
class UConversation;
struct FLine;

UCLASS()
class EDITORTOOLS_API UConversationWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget)) UBorder* AnswersBorder = nullptr;
	UPROPERTY(meta = (BindWidget)) UVerticalBox* AnswersBox = nullptr;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MainTextBlock = nullptr;
	UPROPERTY(meta = (BindWidget)) UTextBlock* NameTextBlock = nullptr;
	UPROPERTY(meta = (BindWidget)) UImage* Portrait = nullptr;
	
	UPROPERTY() TArray<UAnswerWidget*> AnswersArray;
	
	UPROPERTY() UConversation* Conversation = nullptr;
	FLine*  CurrentLine = nullptr;
	FString CurrentText = "";
	int StringLenght = 0;
	FTimerHandle Text_TH;
	
	UPROPERTY(EditDefaultsOnly) int SkipAmount = 5;
	int SkipCount = 0;
	
	int AnswerIndex = 0;
	
	void ChangeLine(int Index);
	void UpdatePortrait();
	void UpdateAnswers();
	void UpdateText();
	
public:
	UPROPERTY(EditDefaultsOnly) class USpeakerDataAsset* SpeakerDA = nullptr;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<UAnswerWidget> AnswerTemplate = nullptr;
	UPROPERTY(EditDefaultsOnly) float LetterSpeed = 0.1f;
	
	UFUNCTION(BlueprintCallable) void StartConversation(UConversation* NewConversation);
	UFUNCTION(BlueprintCallable) void PressUp();
	UFUNCTION(BlueprintCallable) void PressDown();
	UFUNCTION(BlueprintCallable) void PressEnter();
	
};
