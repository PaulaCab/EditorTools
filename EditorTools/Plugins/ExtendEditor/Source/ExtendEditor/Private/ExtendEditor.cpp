// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditor.h"

#include "ContentBrowserModule.h"
#include "DebugHelper.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "SlateWidgets/AdvanceDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FExtendEditorModule"

void FExtendEditorModule::StartupModule()
{
	InitCBMenuExtension();
	RegisterAdvanceDeletionTab();
}

void FExtendEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FExtendEditorModule::InitCBMenuExtension()
{
	auto& cbModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	auto& pathsArray = cbModule.GetAllPathViewContextMenuExtenders();

	//Delegate with return value "TSharedRef<FExtender>" and one param "const TArray<FString>&"
	// FContentBrowserMenuExtender_SelectedPaths customCBMenuDelegate;
	// customCBMenuDelegate.BindRaw(this, &FExtendEditorModule::CustomCBExtender);
	// pathsArray.Add(customCBMenuDelegate);

	pathsArray.Add(FContentBrowserMenuExtender_SelectedPaths::
		CreateRaw(this, &FExtendEditorModule::CustomCBExtender));
}

//Define position for inserting menu entry
TSharedRef<FExtender> FExtendEditorModule::CustomCBExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> menuExtender(new FExtender());

	if(SelectedPaths.Num())
	{
		//If the extension hook doesn't exist there wont be an error, it just wont display
		menuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FExtendEditorModule::AddCBMenuEntry));

		SelectedFolderPaths = SelectedPaths;
	}
	
	return menuExtender;
}

void FExtendEditorModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete unused assets under folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnDeleteUnusedAssetsClicked)
		);

	MenuBuilder.AddMenuEntry(
	FText::FromString(TEXT("Delete Empty Folders")),
	FText::FromString(TEXT("Safely delete all empty folders")),
	FSlateIcon(),
	FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnDeleteEmptyFoldersClicked)
	);

	MenuBuilder.AddMenuEntry(
	FText::FromString(TEXT("Advanced Deletion")),
	FText::FromString(TEXT("Lists assets by specific conditions in a tab for deleting")),
	FSlateIcon(),
	FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnAdvanceDeletionClicked)
);
}

void FExtendEditorModule::OnDeleteUnusedAssetsClicked()
{
	if(SelectedFolderPaths.Num()>1)
	{
		ShowMsg(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}

	auto pathsNames = UEditorAssetLibrary::ListAssets(SelectedFolderPaths[0]);
	if(!pathsNames.Num())
	{
		ShowMsg(EAppMsgType::Ok, TEXT("No assets found under selected folder"));
		return;
	}

	auto result = ShowMsg(EAppMsgType::YesNo, TEXT("A total of ") + FString::FromInt(pathsNames.Num()) + TEXT(" assets found. \nWould you like to procced?"));
	if(result == EAppReturnType::No)
		return;

	TArray<FAssetData> unusedAD;
	for(const FString& path : pathsNames)
	{
		if(path.Contains(TEXT("Developers"))|| path.Contains(TEXT("Collections"))|| path.Contains(TEXT("_External")))
			continue;

		if(!UEditorAssetLibrary::DoesAssetExist(path))
			continue;

		auto assetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(path);
		if(!assetReferences.Num())
		{
			auto data = UEditorAssetLibrary::FindAssetData(path);
			unusedAD.Add(data);
		}
	}

	if(unusedAD.Num())
	{
		ObjectTools::DeleteAssets(unusedAD);
	}
	else
	{
		ShowMsg(EAppMsgType::Ok, TEXT("No unused assets found under selected folder"));
	}
}

void FExtendEditorModule::OnDeleteEmptyFoldersClicked()
{
	TArray<FString> folderPaths = UEditorAssetLibrary::ListAssets(SelectedFolderPaths[0], true,true);
	uint32 counter = 0;

	FString emptyFolderPathsNames;
	TArray<FString> emptyFolderPaths;

	for(const FString& path : folderPaths)
	{
		if(path.Contains(TEXT("Developers"))|| path.Contains(TEXT("Collections"))|| path.Contains(TEXT("__ExternalActors")))
			continue;
		
		if(!UEditorAssetLibrary::DoesDirectoryExist(path))
			continue;

		if(!UEditorAssetLibrary::DoesDirectoryHaveAssets(path))
		{
			emptyFolderPathsNames.Append(path);
			emptyFolderPathsNames.Append(TEXT("\n"));
			
			emptyFolderPaths.Add(path);
		}
	}

	if(!emptyFolderPaths.Num())
	{
		ShowMsg(EAppMsgType::Ok, TEXT("No empty folder found under selected folder"), false);
		return;
	}
	
	EAppReturnType::Type result = ShowMsg(EAppMsgType::OkCancel, TEXT("Empty folders found: \n")
		+ emptyFolderPathsNames + TEXT("\nWould you want to delete all?"), false);

	if(result == EAppReturnType::Cancel)
		return;

	for(const FString& path : emptyFolderPaths)
	{
		if(UEditorAssetLibrary::DeleteDirectory(path))
			++counter;
		else
			Print(TEXT("Failed to delete " +  path) , FColor::Red);
	}

	if(counter)
		ShowNotify(TEXT("Successfully deleted ") + FString::FromInt(counter) + " folders");

}

void FExtendEditorModule::OnAdvanceDeletionClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
}

void FExtendEditorModule::RegisterAdvanceDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvanceDeletion"),
		FOnSpawnTab::CreateRaw(this, &FExtendEditorModule::OnSpawnAdvanceDeletionTab))
		.SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
}

TSharedRef<SDockTab> FExtendEditorModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	//Macro used to create slate widgets
	return SNew(SDockTab).TabRole(NomadTab)
	[
		SNew(SAdvanceDeletionTab)
		.AssetDataToStore(GetAllADUnderSelectedFolder())
	];
}

TArray<TSharedPtr<FAssetData>> FExtendEditorModule::GetAllADUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> availableAD;
	if(!SelectedFolderPaths.Num())
		return availableAD;
	
	auto pathsNames = UEditorAssetLibrary::ListAssets(SelectedFolderPaths[0]);
	
	for(const FString& path : pathsNames)
	{
		if(path.Contains(TEXT("Developers"))|| path.Contains(TEXT("Collections"))|| path.Contains(TEXT("__ExternalActors")))
			continue;

		if(!UEditorAssetLibrary::DoesAssetExist(path))
			continue;

		auto data = UEditorAssetLibrary::FindAssetData(path);
		availableAD.Add(MakeShared<FAssetData>(data));
	}
	
	return availableAD;
}

bool FExtendEditorModule::DeleteSingleAsset(const FAssetData& AssetDataToDelete)
{
	TArray<FAssetData> dataArray;
	dataArray.Add(AssetDataToDelete);
	return ObjectTools::DeleteAssets(dataArray) > 0;
}

bool FExtendEditorModule::DeleteMultipleAssets(const TArray<FAssetData>& AssetDataToDelete)
{
	return ObjectTools::DeleteAssets(AssetDataToDelete)>0;
}

void FExtendEditorModule::ListUnusedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter,
	TArray<TSharedPtr<FAssetData>>& UnusedAD)
{
	UnusedAD.Empty();
	
	for(const auto& data: AssetDataToFilter)
	{
		auto assetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(data->ObjectPath.ToString());

		if(assetReferences.Num() == 0)
		{
			UnusedAD.Add(data);
		}
	}
}

void FExtendEditorModule::ListSameNameAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter,
	TArray<TSharedPtr<FAssetData>>& SameNameAD)
{
	SameNameAD.Empty();
	TMultiMap<FString, TSharedPtr<FAssetData>> assetsInfoMap;

	for(const TSharedPtr<FAssetData>& data : AssetDataToFilter)
	{
		assetsInfoMap.Emplace(data->AssetName.ToString(), data);
	}

	for(const TSharedPtr<FAssetData>& data : AssetDataToFilter)
	{
		TArray<TSharedPtr<FAssetData>> outDA;
		assetsInfoMap.MultiFind(data->AssetName.ToString(), outDA);

		if(outDA.Num()>1)
			SameNameAD.Add(data);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExtendEditorModule, ExtendEditor)