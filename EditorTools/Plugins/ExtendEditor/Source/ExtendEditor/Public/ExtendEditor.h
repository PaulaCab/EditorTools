// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomUICommand/ExtendEditorUICommands.h"
#include "Modules/ModuleManager.h"

class FExtendEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsystem;
	bool GetEditorActorSubsystem();
	
	// CONTENT BROWSER MENU EXTENSION

	TArray<FString> SelectedFolderPaths;
	
	void InitCBMenuExtension();
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetsClicked();
	void OnDeleteEmptyFoldersClicked();
	void OnAdvanceDeletionClicked();

	
	// LEVEL EDITOR MENU EXTENSION
	
	void InitLEMenuExtension();
	TSharedRef<FExtender> CustomLEMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors);
	void AddLEMenuEntry(class FMenuBuilder& MenuBuilder);
	void OnLockActorSelectionClicked();
	void OnUnlockActorSelectionClicked();

	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);
	void LockActorSelection(AActor* Actor);
	void UnlockActorSelection(AActor* Actor);
	bool CheckIsActorSelectionLocked(AActor* Actor);

	
	// DELETION CUSTOM TAB
	
	void RegisterAdvanceDeletionTab();
	TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllADUnderSelectedFolder();

public:
	bool DeleteSingleAsset(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssets(const TArray<FAssetData>& AssetDataToDelete);

	void ListUnusedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& UnusedAD);
	void ListSameNameAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& SameNameAD);

	void SyncCBToClickedAsset(const FString& AssetPath);


	// CUSTOM UI COMMANDS

	TSharedPtr<class FUICommandList> CustomUICommands;
	void InitCustomUICommands();
	void OnLockHotKeyPressed();
	void OnUnlockHotKeyPressed();
};

