// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Components/PawnComponent.h"

#include "<PROJECT_NAME>InventoryManagerComponent.generated.h"

class U<PROJECT_NAME>InventoryItemDefinition;
class U<PROJECT_NAME>InventoryItemInstance;
class U<PROJECT_NAME>InventoryManagerComponent;
struct F<PROJECT_NAME>InventoryList;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>InventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	UActorComponent* InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	U<PROJECT_NAME>InventoryItemInstance* Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	F<PROJECT_NAME>InventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend F<PROJECT_NAME>InventoryList;
	friend U<PROJECT_NAME>InventoryManagerComponent;

	UPROPERTY()
	U<PROJECT_NAME>InventoryItemInstance* Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>InventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	F<PROJECT_NAME>InventoryList()
		: OwnerComponent(nullptr)
	{
	}

	F<PROJECT_NAME>InventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<U<PROJECT_NAME>InventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<F<PROJECT_NAME>InventoryEntry, F<PROJECT_NAME>InventoryList>(Entries, DeltaParms, *this);
	}

	U<PROJECT_NAME>InventoryItemInstance* AddEntry(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(U<PROJECT_NAME>InventoryItemInstance* Instance);

	void RemoveEntry(U<PROJECT_NAME>InventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(F<PROJECT_NAME>InventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend U<PROJECT_NAME>InventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<F<PROJECT_NAME>InventoryEntry> Entries;

	UPROPERTY()
	UActorComponent* OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<F<PROJECT_NAME>InventoryList> : public TStructOpsTypeTraitsBase2<F<PROJECT_NAME>InventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class U<PROJECT_NAME>InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	U<PROJECT_NAME>InventoryItemInstance* AddItemDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(U<PROJECT_NAME>InventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(U<PROJECT_NAME>InventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<U<PROJECT_NAME>InventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	U<PROJECT_NAME>InventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	F<PROJECT_NAME>InventoryList InventoryList;
};
