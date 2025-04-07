// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditor.h"

#include "ContentBrowserModule.h"
#include "DebugHelper.h"
#include "EditorAssetLibrary.h"
#include "LevelEditor.h"
#include "ObjectTools.h"
#include "SceneOutlinerModule.h"
#include "SceneOutlinerPublicTypes.h"
#include "Selection.h"
#include "../CustomStyle/ExtendEditorStyle.h"
#include "CustomOutlinerColumn/OutlinerLockColumn.h"
#include "CustomUICommand/ExtendEditorUICommands.h"
#include "SlateWidgets/AdvanceDeletionWidget.h"
#include "Subsystems/EditorActorSubsystem.h"

#define LOCTEXT_NAMESPACE "FExtendEditorModule"

void FExtendEditorModule::StartupModule()
{
	FExtendEditorStyle::InitializeIcons();
	FExtendEditorUICommands::Register();
	InitCustomUICommands();
	InitCBMenuExtension();
	InitLEMenuExtension();
	RegisterAdvanceDeletionTab();
	InitCustomSelectionEvent();
	InitSceneOutlinerColumnExtension();
}

void FExtendEditorModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvanceDeletion"));
	FExtendEditorStyle::ShutDown();
	FExtendEditorUICommands::Unregister();
	UnregisterSceneOutlinerColumnExtension();
}

bool FExtendEditorModule::GetEditorActorSubsystem()
{
	if(!WeakEditorActorSubsystem.IsValid())
	{
		WeakEditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	return WeakEditorActorSubsystem.IsValid();
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
		CreateRaw(this, &FExtendEditorModule::CustomCBMenuExtender));
}

//Define position for inserting menu entry
TSharedRef<FExtender> FExtendEditorModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
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
		FSlateIcon(FExtendEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteAssets"),
		FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnDeleteUnusedAssetsClicked)
		);

	MenuBuilder.AddMenuEntry(
	FText::FromString(TEXT("Delete Empty Folders")),
	FText::FromString(TEXT("Safely delete all empty folders")),
	FSlateIcon(FExtendEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteAssets"),
	FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnDeleteEmptyFoldersClicked)
	);

	MenuBuilder.AddMenuEntry(
	FText::FromString(TEXT("Advanced Deletion")),
	FText::FromString(TEXT("Lists assets by specific conditions in a tab for deleting")),
	FSlateIcon(FExtendEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteAssets"),
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

#pragma region Deletion Tab

void FExtendEditorModule::OnAdvanceDeletionClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
}

void FExtendEditorModule::RegisterAdvanceDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvanceDeletion"),
		FOnSpawnTab::CreateRaw(this, &FExtendEditorModule::OnSpawnAdvanceDeletionTab))
		.SetDisplayName(FText::FromString(TEXT("Advance Deletion")))
		.SetIcon(FSlateIcon(FExtendEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteAssets"));
}

TSharedRef<SDockTab> FExtendEditorModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FString path = SelectedFolderPaths.Num()==0 ? "" : SelectedFolderPaths[0];
	//Macro used to create slate widgets
	return SNew(SDockTab).TabRole(NomadTab)
	[
		SNew(SAdvanceDeletionTab)
		.AssetDataToStore(GetAllADUnderSelectedFolder())
		.SelectedFolder(path)
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
		auto assetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(data->GetSoftObjectPath().ToString());

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

void FExtendEditorModule::SyncCBToClickedAsset(const FString& AssetPath)
{
	TArray<FString> assetsPathToSync;
	assetsPathToSync.Add(AssetPath);
	UEditorAssetLibrary::SyncBrowserToObjects(assetsPathToSync);
}

#pragma endregion 

void FExtendEditorModule::InitLEMenuExtension()
{
	auto& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	auto& menuExtendersArray = levelEditorModule.GetAllLevelViewportContextMenuExtenders();
	auto levelCommands = levelEditorModule.GetGlobalLevelEditorActions();
	levelCommands->Append(CustomUICommands.ToSharedRef());

	menuExtendersArray.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::
		CreateRaw(this, &FExtendEditorModule::CustomLEMenuExtender ));
}

TSharedRef<FExtender> FExtendEditorModule::CustomLEMenuExtender(const TSharedRef<FUICommandList> UICommandList,
	const TArray<AActor*> SelectedActors)
{
	TSharedRef<FExtender> menuExtender = MakeShareable(new FExtender);
	
	if(SelectedActors.Num()>0)
	{
		menuExtender->AddMenuExtension(
			FName("ActorOptions"),
			EExtensionHook::Before,
			UICommandList,
			FMenuExtensionDelegate::CreateRaw(this, &FExtendEditorModule::AddLEMenuEntry));
	}
	return menuExtender;
}

void FExtendEditorModule::AddLEMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Lock Actor Selection")),
		FText::FromString(TEXT("Prevent actor from been selected")),
		FSlateIcon(FExtendEditorStyle::GetStyleSetName(), "LevelEditor.LockActors"),
		FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnLockActorSelectionClicked)
	);

	MenuBuilder.AddMenuEntry(
	FText::FromString(TEXT("Unlock Actor Selection")),
	FText::FromString(TEXT("Remove the selection constraint")),
	FSlateIcon(FExtendEditorStyle::GetStyleSetName(), "LevelEditor.LockActors"),
	FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnUnlockActorSelectionClicked)
);
}

void FExtendEditorModule::OnLockActorSelectionClicked()
{
	if(!GetEditorActorSubsystem())
		return;

	TArray<AActor*> selectedActors = WeakEditorActorSubsystem->GetSelectedLevelActors();
	if(!selectedActors.Num())
	{
		ShowNotify(TEXT("No actor selected"));
		return;
	}

	FString currentLockedActorNames = TEXT("Locked selection for:");
	for(AActor* actor: selectedActors)
	{
		if(!actor)
			continue;

		LockActorSelection(actor);
		WeakEditorActorSubsystem->SetActorSelectionState(actor,false);
		currentLockedActorNames.Append(TEXT("\n"));
		currentLockedActorNames.Append(actor->GetActorLabel());
	}

	RefreshSceneOuliner();
	ShowNotify(currentLockedActorNames);
}

void FExtendEditorModule::OnUnlockActorSelectionClicked()
{
	if(!GetEditorActorSubsystem())
		return;

	TArray<AActor*> allActors = WeakEditorActorSubsystem->GetAllLevelActors();
	TArray<AActor*> allLockedActors;
	for(AActor* actor : allActors)
	{
		if(CheckIsActorSelectionLocked(actor))
			allLockedActors.Add(actor);
	}

	if(!allLockedActors.Num())
	{
		ShowNotify(TEXT("No selection locked actor currently"));
		return;
	}

	FString unlockActorNames = TEXT("Lifted selection constraint for:");
	for(AActor* lockedActor : allLockedActors)
	{
		UnlockActorSelection(lockedActor);
		unlockActorNames.Append(TEXT("\n"));
		unlockActorNames.Append(lockedActor->GetActorLabel());
	}

	RefreshSceneOuliner();
	ShowNotify(unlockActorNames);
}

void FExtendEditorModule::InitCustomSelectionEvent()
{
	USelection* userSelection = GEditor->GetSelectedActors();
	userSelection->SelectObjectEvent.AddRaw(this, &FExtendEditorModule::OnActorSelected);
}

void FExtendEditorModule::OnActorSelected(UObject* SelectedObject)
{
	if(AActor* selectedActor = Cast<AActor>(SelectedObject))
	{
		if(CheckIsActorSelectionLocked(selectedActor))
			WeakEditorActorSubsystem->SetActorSelectionState(selectedActor, false);			
	}
}

void FExtendEditorModule::LockActorSelection(AActor* Actor)
{
	if(!Actor)
		return;

	if(!Actor->ActorHasTag(FName("Locked")))
		Actor->Tags.Add(FName("Locked"));
}

void FExtendEditorModule::UnlockActorSelection(AActor* Actor)
{
	if(!Actor)
		return;

	if(Actor->ActorHasTag(FName("Locked")))
		Actor->Tags.Remove(FName("Locked"));
}

bool FExtendEditorModule::CheckIsActorSelectionLocked(AActor* Actor)
{
	if(!Actor)
		return false;

	return Actor->ActorHasTag(FName("Locked"));
}

void FExtendEditorModule::ProcessLockingForOuliner(AActor* Actor, bool bShouldLock)
{
	if(!GetEditorActorSubsystem())
		return;
	
	if(bShouldLock)
	{
		LockActorSelection(Actor);
		WeakEditorActorSubsystem->SetActorSelectionState(Actor, false);
	}
	else
		UnlockActorSelection(Actor);
}

void FExtendEditorModule::InitCustomUICommands()
{
	CustomUICommands = MakeShareable(new FUICommandList());
	
	CustomUICommands->MapAction(FExtendEditorUICommands::Get().LockActorSelection,
		FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnLockHotKeyPressed));

	CustomUICommands->MapAction(FExtendEditorUICommands::Get().UnlockActorSelection,
		FExecuteAction::CreateRaw(this, &FExtendEditorModule::OnUnlockHotKeyPressed));

}

void FExtendEditorModule::OnLockHotKeyPressed()
{
	OnLockActorSelectionClicked();
}

void FExtendEditorModule::OnUnlockHotKeyPressed()
{
	OnUnlockActorSelectionClicked();
}

void FExtendEditorModule::InitSceneOutlinerColumnExtension()
{
	auto& sceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));

	FSceneOutlinerColumnInfo lockColumnInfo(
	ESceneOutlinerColumnVisibility::Visible,
	1,
	FCreateSceneOutlinerColumn::CreateRaw(this, &FExtendEditorModule::OnCreateLockColumn)
	);

	sceneOutlinerModule.RegisterDefaultColumnType<FOutlinerLockColumn>(lockColumnInfo);
}

void FExtendEditorModule::UnregisterSceneOutlinerColumnExtension()
{
	auto& sceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));
	sceneOutlinerModule.UnRegisterColumnType<FOutlinerLockColumn>();
}

TSharedRef<ISceneOutlinerColumn> FExtendEditorModule::OnCreateLockColumn(ISceneOutliner & SceneOutliner)
{
	return MakeShareable(new FOutlinerLockColumn(SceneOutliner));
}

void FExtendEditorModule::RefreshSceneOuliner()
{
	auto& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	auto sceneOutliner = levelEditorModule.GetFirstLevelEditor()->GetSceneOutliner();

	if(sceneOutliner.IsValid())
	{
		sceneOutliner->FullRefresh();
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExtendEditorModule, ExtendEditor)