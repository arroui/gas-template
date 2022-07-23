// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>VerbMessageReplication.h"
#include "GameFramework/GameplayMessageSubsystem.h"

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>VerbMessageReplicationEntry

FString F<PROJECT_NAME>VerbMessageReplicationEntry::GetDebugString() const
{
	return Message.ToString();
}

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>VerbMessageReplication

void F<PROJECT_NAME>VerbMessageReplication::AddMessage(const F<PROJECT_NAME>VerbMessage& Message)
{
	F<PROJECT_NAME>VerbMessageReplicationEntry& NewStack = CurrentMessages.Emplace_GetRef(Message);
	MarkItemDirty(NewStack);
}

void F<PROJECT_NAME>VerbMessageReplication::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
// 	for (int32 Index : RemovedIndices)
// 	{
// 		const FGameplayTag Tag = CurrentMessages[Index].Tag;
// 		TagToCountMap.Remove(Tag);
// 	}
}

void F<PROJECT_NAME>VerbMessageReplication::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const F<PROJECT_NAME>VerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void F<PROJECT_NAME>VerbMessageReplication::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const F<PROJECT_NAME>VerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void F<PROJECT_NAME>VerbMessageReplication::RebroadcastMessage(const F<PROJECT_NAME>VerbMessage& Message)
{
	check(Owner);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(Owner);
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}
