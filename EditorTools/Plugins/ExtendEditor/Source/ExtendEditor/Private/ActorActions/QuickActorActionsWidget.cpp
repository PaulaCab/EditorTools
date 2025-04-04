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

void UQuickActorActionsWidget::DuplicateActors()
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

	if(NumberOfDuplicates<=0 || OffsetDist == 0)
	{
		ShowNotify(TEXT("Did not specify a number of duplications or an offset distance"));
		return;
	}

	for(AActor* actor : selectedActors)
	{
		if(!actor)
			continue;

		for(int32 i = 0; i<NumberOfDuplicates; i++)
		{
			AActor* duplicate = EditorActorSubsystem->DuplicateActor(actor, actor->GetWorld());

			if(!duplicate)
				continue;

			const float dist = (i+1)*OffsetDist;

			switch (DuplicationAxis)
			{
			case E_DuplicationAxis::EDA_XAxis:
				duplicate->AddActorWorldOffset(FVector(dist,0.f,0.f));
				break;
			case E_DuplicationAxis::EDA_YAxis:
				duplicate->AddActorWorldOffset(FVector(0.f,dist,0.f));
				break;
			case E_DuplicationAxis::EDA_ZAxis:
				duplicate->AddActorWorldOffset(FVector(0.f,0.f,dist));
				break;
			case E_DuplicationAxis::EDA_MAX:
				break;
			}

			EditorActorSubsystem->SetActorSelectionState(duplicate, true);
			counter++;
		}
	}

	if(counter>0)
		ShowNotify(TEXT("Successfully duplicated ") + FString::FromInt(counter) + TEXT(" actors"));
}
