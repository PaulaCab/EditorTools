// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversation.h"

void UConversation::DeleteLine(FLine& Line)
{
	if(Lines.Num()<2)
		return;

	int index = Lines.IndexOfByPredicate([&](const FLine& item){return item == Line;});
	Lines.RemoveAt(index);
}

void UConversation::NewLine( FLine& Line)
{
	int index = Lines.IndexOfByPredicate([&](const FLine& item){return item == Line;});
	
	if(index == Lines.Num()-1 && Line.NextLine == -1)
		Line.NextLine = Lines.Num();
	
	if(index == INDEX_NONE)
		index = Lines.Num()-1;
	
	Lines.Insert(FLine(), index+1);
	if(index<Lines.Num()-2)
	Lines[index+1].NextLine = index+2;
}
