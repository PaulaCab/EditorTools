// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISceneOutliner.h"
#include "ISceneOutlinerColumn.h"

class  FOutlinerLockColumn : public ISceneOutlinerColumn
{
public:
	FOutlinerLockColumn(ISceneOutliner& SceneOutliner){}

	static FName GetID() {return FName("SelectionLock");}
	virtual FName GetColumnID() override {return FName("SelectionLock");}
	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef< SWidget > ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;

private:
	void OnRowWidgetStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> Actor);
};
