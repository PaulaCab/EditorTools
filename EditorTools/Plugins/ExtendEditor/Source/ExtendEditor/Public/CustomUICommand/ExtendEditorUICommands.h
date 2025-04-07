// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"


class  FExtendEditorUICommands : public TCommands<FExtendEditorUICommands>
{
public:
	FExtendEditorUICommands() : TCommands<FExtendEditorUICommands> (
		TEXT("ExtendEditor"),
		FText::FromString(TEXT("Extend Editor UI Commands"))
		,NAME_None,
		TEXT("ExtendEditor")
		){}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> LockActorSelection;
	TSharedPtr<FUICommandInfo> UnlockActorSelection;

};
