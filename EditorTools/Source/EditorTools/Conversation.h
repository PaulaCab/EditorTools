// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Conversation.generated.h"


UENUM()
enum class ESpeaker : int8
{
	Alice,
	Nico
};

UENUM()
enum class EEmotion : int8
{
	Calm,
	Happy,
	Angry,
	Blush,
	Sad
};

USTRUCT()
struct FAnswer
{
	GENERATED_BODY()
	
	UPROPERTY() FText Text;
	UPROPERTY() int NextLine = -1;
};

USTRUCT()
struct FLine
{
	GENERATED_BODY()
	
	UPROPERTY() ESpeaker Speaker = ESpeaker::Alice;
	UPROPERTY() EEmotion Emotion = EEmotion::Happy;
	UPROPERTY() FText Text;
	UPROPERTY() bool bHasAnswer = false;
	UPROPERTY() int NextLine = -1;
	UPROPERTY() TArray<FAnswer> Answers;
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
	void NewLine(FLine& Line);
	void SwitchLine(FLine& Line, int Step);

	void NewAnswer(FLine& Line);
	void DeleteAnswer(FLine& Line);

private:
	void UpdateNextLineIndex(int Start, int Step);
};
