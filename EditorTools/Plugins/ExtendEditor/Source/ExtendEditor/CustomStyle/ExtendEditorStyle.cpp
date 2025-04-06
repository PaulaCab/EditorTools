// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendEditorStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FExtendEditorStyle::StyleSetName = FName("ExtendedEditorStyle");
TSharedPtr<FSlateStyleSet> FExtendEditorStyle::StyleSet = nullptr;

void FExtendEditorStyle::InitializeIcons()
{
	if(!StyleSet.IsValid())
	{
		StyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}
}

void FExtendEditorStyle::ShutDown()
{
	if(StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
		StyleSet.Reset();
	}
}

TSharedRef<FSlateStyleSet> FExtendEditorStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> customStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));
	const FString iconDirectory = IPluginManager::Get().FindPlugin(TEXT("ExtendEditor"))->GetBaseDir()/"Resources";
	const FVector2d icon16x16(16.f,16.f);
	customStyleSet->SetContentRoot(iconDirectory);

	//register delete icon
	customStyleSet->Set("ContentBrowser.DeleteAssets",
		new FSlateImageBrush(iconDirectory/"DeleteIcon.png", icon16x16));

	//register lock icon
	customStyleSet->Set("LevelEditor.LockActors",
		new FSlateImageBrush(iconDirectory/"LockIcon.png", icon16x16));
	
	return customStyleSet;
}
