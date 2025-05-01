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

	FLine newLine = FLine();
	
	Lines.Insert(newLine, index+1);
	if(index<Lines.Num()-2)
		newLine.NextLine = index+2;
}

void UConversation::NewAnswer(FLine& Line)
{
	FAnswer answer = FAnswer();
	Line.Answers.Add(FAnswer());
	
}

void UConversation::DeleteAnswer(FLine& Line)
{
	if(Line.Answers.Num())
		Line.Answers.RemoveAt(Line.Answers.Num()-1);
}