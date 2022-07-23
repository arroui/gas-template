// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySet.h"
#include "Components/PawnComponent.h"

#include "<PROJECT_NAME>EquipmentManagerComponent.generated.h"

class U<PROJECT_NAME>EquipmentDefinition;
class U<PROJECT_NAME>EquipmentInstance;
class U<PROJECT_NAME>AbilitySystemComponent;
struct F<PROJECT_NAME>EquipmentList;
class U<PROJECT_NAME>EquipmentManagerComponent;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>AppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	F<PROJECT_NAME>AppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend F<PROJECT_NAME>EquipmentList;
	friend U<PROJECT_NAME>EquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	U<PROJECT_NAME>EquipmentInstance* Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	F<PROJECT_NAME>AbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>EquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	F<PROJECT_NAME>EquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	F<PROJECT_NAME>EquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<F<PROJECT_NAME>AppliedEquipmentEntry, F<PROJECT_NAME>EquipmentList>(Entries, DeltaParms, *this);
	}

	U<PROJECT_NAME>EquipmentInstance* AddEntry(TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(U<PROJECT_NAME>EquipmentInstance* Instance);

private:
	U<PROJECT_NAME>AbilitySystemComponent* GetAbilitySystemComponent() const;

	friend U<PROJECT_NAME>EquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<F<PROJECT_NAME>AppliedEquipmentEntry> Entries;

	UPROPERTY()
	UActorComponent* OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<F<PROJECT_NAME>EquipmentList> : public TStructOpsTypeTraitsBase2<F<PROJECT_NAME>EquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class U<PROJECT_NAME>EquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	U<PROJECT_NAME>EquipmentInstance* EquipItem(TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(U<PROJECT_NAME>EquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	U<PROJECT_NAME>EquipmentInstance* GetFirstInstanceOfType(TSubclassOf<U<PROJECT_NAME>EquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<U<PROJECT_NAME>EquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<U<PROJECT_NAME>EquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	F<PROJECT_NAME>EquipmentList EquipmentList;
};
