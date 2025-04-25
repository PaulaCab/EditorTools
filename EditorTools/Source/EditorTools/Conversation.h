// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Conversation.generated.h"


UENUM()
enum class ESpeaker : int8
{
	Speaker1,
	Speaker2
};

UENUM()
enum class EEmotion : int8
{
	Happy,
	Angry,
	Sad
};

USTRUCT()
struct FLine
{
	GENERATED_BODY()
	
	ESpeaker Speaker = ESpeaker::Speaker1;
	EEmotion Emotion = EEmotion::Happy;
	FText Text;
	bool bHasAnswer = false;
	int NextLine = -1;
	TArray<TPair<FText, int>> Answers;
	
};

UCLASS()
class EDITORTOOLS_API UConversation : public UDataAsset
{
	GENERATED_BODY()

public:
	FString ConversationName = "Placeholder";
	TArray<FLine> Lines = {FLine()};
};
