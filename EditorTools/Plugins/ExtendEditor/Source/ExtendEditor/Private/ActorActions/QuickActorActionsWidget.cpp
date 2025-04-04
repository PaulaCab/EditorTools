// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/QuickActorActionsWidget.h"

#include "DebugHelper.h"
#include "Subsystems/EditorActorSubsystem.h"


bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if(!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	
	return EditorActorSubsystem!=nullptr;
}

void UQuickActorActionsWidget::SelectAllActorsWithSimilarName()
{
	if(!GetEditorActorSubsystem())
		return;
	
	TArray<AActor*> selectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 counter = 0;

	if(!selectedActors.Num())
	{
		ShowNotify(TEXT("No actor selected"));
		return;
	}

	if(selectedActors.Num()>1)
	{
		ShowNotify(TEXT("You can only select one actor"));
		return;
	}

	FString actorName = selectedActors[0]->GetActorLabel();
	const FString nameToSearch = actorName.LeftChop(4);

	TArray<AActor*> allLevelActors = EditorActorSubsystem->GetAllLevelActors();
	for(AActor* actor : allLevelActors)
	{
		if(!actor)
			continue;

		if(actor->GetActorLabel().Contains(nameToSearch, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(actor, true);
			counter++;
		}
	}

	if(counter>0)
		ShowNotify(TEXT("Successfully selected ") + FString::FromInt(counter) + TEXT(" actors"));
	else
		ShowNotify(TEXT("No actors with similar name found"));
}
