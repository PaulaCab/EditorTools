// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomOutlinerColumn/OutlinerLockColumn.h"

#include "ActorTreeItem.h"
#include "ExtendEditor.h"
#include "ISceneOutlinerTreeItem.h"
#include "ExtendEditor/CustomStyle/ExtendEditorStyle.h"

SHeaderRow::FColumn::FArguments FOutlinerLockColumn::ConstructHeaderRowColumn()
{
	auto headerRow = SHeaderRow::Column(GetColumnID())
	.FixedWidth(24.f)
	.HAlignHeader(HAlign_Center)
	.VAlignHeader(VAlign_Center)
	.HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.DefaultTooltip(FText::FromString(TEXT("Actor Selection Lock - Press icon to lock actor selection")))
	[
		SNew(SImage)
		.ColorAndOpacity(FSlateColor::UseForeground())
		.Image(FExtendEditorStyle::GetStyleSet()->GetBrush(FName("LevelEditor.LockActors")))
	];
	
	return headerRow;
}

const TSharedRef<SWidget> FOutlinerLockColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
	const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	auto* actorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if(!actorTreeItem || !actorTreeItem->IsValid())
		return SNullWidget::NullWidget;

	auto& editorModule = FModuleManager::LoadModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));
	const bool isActorLocked = editorModule.CheckIsActorSelectionLocked(actorTreeItem->Actor.Get());

	const auto& style = FExtendEditorStyle::GetStyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName("SceneOutliner.SelectionLock"));

	TSharedRef<SCheckBox> rowWidget =SNew(SCheckBox)
	.Visibility(EVisibility::Visible)
	.Type(ESlateCheckBoxType::ToggleButton)
	.Style(&style)
	.HAlign(HAlign_Center)
	.IsChecked(isActorLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
	.OnCheckStateChanged(this, &FOutlinerLockColumn::OnRowWidgetStateChanged,actorTreeItem->Actor);
	
	return rowWidget;
}

void FOutlinerLockColumn::OnRowWidgetStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> Actor)
{
	auto& editorModule = FModuleManager::LoadModuleChecked<FExtendEditorModule>(TEXT("ExtendEditor"));
	
	switch(NewState) {
	case ECheckBoxState::Unchecked:
		editorModule.ProcessLockingForOuliner(Actor.Get(), false);
		break;
	case ECheckBoxState::Checked:
		editorModule.ProcessLockingForOuliner(Actor.Get(), true);
		break;
	case ECheckBoxState::Undetermined:
		break;
	}

}
