// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversation.h"
#include "Engine/DataAsset.h"
#include "SpeakerDataAsset.generated.h"


UCLASS()
class EDITORTOOLS_API UEmotionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) FText CharacterName;
	UPROPERTY(EditAnywhere) TMap<EEmotion, UTexture2D*> EmotionMap = {
		{EEmotion::Calm, nullptr},
		{EEmotion::Happy, nullptr},
		{EEmotion::Angry, nullptr},
		{EEmotion::Blush, nullptr},
		{EEmotion::Sad, nullptr}
	};	
};


UCLASS()
class EDITORTOOLS_API USpeakerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) TMap<ESpeaker, UEmotionDataAsset*> SpeakerMap = {
		{ESpeaker::Alice, nullptr},
		{ESpeaker::Nico, nullptr}
	};	
};
