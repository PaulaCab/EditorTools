// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickAssetAction.h"
#include "DebugHelper.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "ObjectTools.h"


void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> selectedAD = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> unusedAD;

	for(const FAssetData& data : selectedAD)
	{
		TArray<FString> assetsReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(data.GetSoftObjectPath().ToString());

		if(!assetsReferences.Num())
			unusedAD.Add(data);
	}

	if(!unusedAD.Num())
	{
		ShowMsg(EAppMsgType::Type::Ok, TEXT("No unused asset found"));
		return;
	}

	const int32 deletedNum = ObjectTools::DeleteAssets(unusedAD);

	if(deletedNum>0)
		ShowNotify(TEXT("Deleted ") + FString::FromInt(deletedNum)+ TEXT(" unused assets"));
}

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
