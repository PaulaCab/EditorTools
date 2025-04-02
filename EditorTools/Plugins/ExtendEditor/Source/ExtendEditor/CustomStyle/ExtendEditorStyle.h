// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FExtendEditorStyle
{
public:
	static void InitializeIcons();
	static void ShutDown();

private:
	static FName StyleSetName;

	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> StyleSet;

public:
	static FName GetStyleSetName(){return StyleSetName;}
};