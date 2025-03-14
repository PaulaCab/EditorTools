// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickAssetAction.h"
#include "DebugHelper.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"



void UQuickAssetAction::DuplicateAssets(int32 Num)
{
	if(Num<=0)
	{
		Print(TEXT("INVALID number"), FColor::Red);
		return;
	}

	TArray<FAssetData> selectedAD = UEditorUtilityLibrary::GetSelectedAssetData();
	for(const FAssetData& data : selectedAD)
	{
		for(int32 i = 0; i<Num; i++)
		{
			const FString sourceAssetPath = data.GetSoftObjectPath().ToString();
			const FString newAssetName = data.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			const FString newPathName = FPaths::Combine(data.PackagePath.ToString(), newAssetName);

			if(UEditorAssetLibrary::DuplicateAsset(sourceAssetPath, newPathName))
				UEditorAssetLibrary::SaveAsset(newPathName, false);
		}
	}
}
