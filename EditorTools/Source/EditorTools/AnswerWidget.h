// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnswerWidget.generated.h"


/**
 * 
 */
UCLASS()
class EDITORTOOLS_API UAnswerWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget)) class UTextBlock* AnswerTextBlock = nullptr;
	
	virtual void NativePreConstruct() override;

public:
	UPROPERTY(EditAnywhere) FText AnswerText;

	void SetText(const FText& Text);
	void OnSelect();
	void OnDiselect();
};
