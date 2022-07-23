// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "<PROJECT_NAME>VerbMessage.h"
#include "<PROJECT_NAME>VerbMessageReplication.generated.h"

struct F<PROJECT_NAME>VerbMessageReplication;

/**
 * Represents one verb message
 */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>VerbMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	F<PROJECT_NAME>VerbMessageReplicationEntry()
	{}

	F<PROJECT_NAME>VerbMessageReplicationEntry(const F<PROJECT_NAME>VerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend F<PROJECT_NAME>VerbMessageReplication;

	UPROPERTY()
	F<PROJECT_NAME>VerbMessage Message;
};

/** Container of verb messages to replicate */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>VerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

	F<PROJECT_NAME>VerbMessageReplication()
	{
	}

public:
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	// Broadcasts a message from server to clients
	void AddMessage(const F<PROJECT_NAME>VerbMessage& Message);

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<F<PROJECT_NAME>VerbMessageReplicationEntry, F<PROJECT_NAME>VerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	void RebroadcastMessage(const F<PROJECT_NAME>VerbMessage& Message);

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<F<PROJECT_NAME>VerbMessageReplicationEntry> CurrentMessages;
	
	// Owner (for a route to a world)
	UPROPERTY()
	UObject* Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<F<PROJECT_NAME>VerbMessageReplication> : public TStructOpsTypeTraitsBase2<F<PROJECT_NAME>VerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
