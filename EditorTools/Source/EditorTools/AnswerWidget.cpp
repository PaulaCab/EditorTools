// Fill out your copyright notice in the Description page of Project Settings.


#include "AnswerWidget.h"

#include "Components/TextBlock.h"

void UAnswerWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(AnswerTextBlock)
		AnswerTextBlock->SetText(AnswerText);
}

void UAnswerWidget::SetText(const FText& Text)
{
	AnswerText = Text;

	if(AnswerTextBlock)
		AnswerTextBlock->SetText(AnswerText);
}

void UAnswerWidget::OnSelect()
{
	if(AnswerTextBlock)
		AnswerTextBlock->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
}

void UAnswerWidget::OnDiselect()
{
	if(AnswerTextBlock)
		AnswerTextBlock->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.f));
}
