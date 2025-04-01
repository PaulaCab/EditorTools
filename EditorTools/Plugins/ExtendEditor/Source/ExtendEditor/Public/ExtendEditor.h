// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FExtendEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	// CONTENT BROWSER MENU EXTENSION

	TArray<FString> SelectedFolderPaths;
	
	void InitCBMenuExtension();
	TSharedRef<FExtender> CustomCBExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetsClicked();
	void OnDeleteEmptyFoldersClicked();
	void OnAdvanceDeletionClicked();

	// deletion custom tab
	
	void RegisterAdvanceDeletionTab();
	TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllADUnderSelectedFolder();

public:
	bool DeleteSingleAsset(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssets(const TArray<FAssetData>& AssetDataToDelete);

	void ListUnusedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& UnusedAD);
	void ListSameNameAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& SameNameAD);

	
};
