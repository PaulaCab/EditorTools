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

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> selectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	for(UObject* object : selectedObjects)
	{
		if(!object)
			continue;

		FString* prefix = PrefixMap.Find(object->GetClass());
		if(!prefix|| prefix->IsEmpty())
		{
			Print(TEXT("Failed to find prefix for class " + object->GetClass()->GetName()), FColor::Red);
			continue;
		}

		FString oldName = object->GetName();
		if(oldName.StartsWith(*prefix))
		{
			Print(oldName+ TEXT(" already has prefix"), FColor::Red);
			continue;
		}
		if(object->IsA<UMaterialInstanceConstant>())
		{
			oldName.RemoveFromStart(TEXT("M_"));
			oldName.RemoveFromEnd("_Inst");
		}

		const FString newName = *prefix + oldName;

		UEditorUtilityLibrary::RenameAsset(object, newName);
	}
}
