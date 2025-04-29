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
	
	UPROPERTY() ESpeaker Speaker = ESpeaker::Speaker1;
	UPROPERTY() EEmotion Emotion = EEmotion::Happy;
	UPROPERTY() FText Text;
	UPROPERTY() bool bHasAnswer = false;
	UPROPERTY() int NextLine = -1;
	TArray<TPair<FText, int>> Answers;	
};

FORCEINLINE bool operator==(const FLine& A, const FLine& B)
{
	if (A.Speaker != B.Speaker) return false;
	if (A.Emotion != B.Emotion) return false;
	if (!A.Text.EqualTo(B.Text)) return false;
	if (A.NextLine != B.NextLine) return false;
	return true;
}

UCLASS()
class EDITORTOOLS_API UConversation : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY() TArray<FLine> Lines = {FLine()};

	void DeleteLine(FLine& Line);
	void NewLine( FLine& Line);
};
