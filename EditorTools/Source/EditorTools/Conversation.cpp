// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversation.h"


void UConversation::DeleteLine(FLine& Line)
{
	if(Lines.Num()<2)
		return;

	int index = Lines.IndexOfByPredicate([&](const FLine& item){return item == Line;});
	Lines.RemoveAt(index);

	UpdateNextLineIndex(index, -1);
	
}

void UConversation::NewLine(FLine& Line)
{
 	int index = Lines.IndexOfByPredicate([&](const FLine& item){return item == Line;});
	
	if(index == Lines.Num()-1 && Line.NextLine == -1)
		Line.NextLine = Lines.Num();
	
	if(index == INDEX_NONE)
		index = Lines.Num()-1;

	FLine newLine = FLine();

	index++;
	Lines.Insert(newLine, index);

	UpdateNextLineIndex(index, 1);
	if(index<Lines.Num()-1)
		Lines[index].NextLine = index+1;
}

void UConversation::SwitchLine(FLine& Line, int Step)
{
	int index = Lines.IndexOfByPredicate([&](const FLine& item){return item == Line;});
	
	if(!Lines.IsValidIndex(index+Step))
		return;

	Lines.Swap(index, index+Step);

	int aux = Lines[index].NextLine;
	Lines[index].NextLine = Lines[index+Step].NextLine;
	Lines[index+Step].NextLine = aux;
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

//if next line index is over the one where a line has been added or deleted,
//the index will update to keep referencing same lines
void UConversation::UpdateNextLineIndex(int Start, int Step)
{
	for(auto& line : Lines)
	{
		if(line.NextLine!=-1 && line.NextLine>Start)
			line.NextLine+=Step;
		
		for(auto& answer : line.Answers)
		{
			if(answer.NextLine!=-1 && answer.NextLine>Start)
				answer.NextLine+=Step;
		}
	}
}
