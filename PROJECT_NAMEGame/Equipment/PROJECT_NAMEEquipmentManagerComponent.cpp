// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>EquipmentManagerComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySet.h"
#include "<PROJECT_NAME>EquipmentInstance.h"
#include "<PROJECT_NAME>EquipmentDefinition.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>AppliedEquipmentEntry

FString F<PROJECT_NAME>AppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>EquipmentList

void F<PROJECT_NAME>EquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
 	for (int32 Index : RemovedIndices)
 	{
 		const F<PROJECT_NAME>AppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
 	}
}

void F<PROJECT_NAME>EquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const F<PROJECT_NAME>AppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void F<PROJECT_NAME>EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
// 	for (int32 Index : ChangedIndices)
// 	{
// 		const FGameplayTagStack& Stack = Stacks[Index];
// 		TagToCountMap[Stack.Tag] = Stack.StackCount;
// 	}
}

U<PROJECT_NAME>AbilitySystemComponent* F<PROJECT_NAME>EquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<U<PROJECT_NAME>AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

U<PROJECT_NAME>EquipmentInstance* F<PROJECT_NAME>EquipmentList::AddEntry(TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipmentDefinition)
{
	U<PROJECT_NAME>EquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
 	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const U<PROJECT_NAME>EquipmentDefinition* EquipmentCDO = GetDefault<U<PROJECT_NAME>EquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<U<PROJECT_NAME>EquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = U<PROJECT_NAME>EquipmentInstance::StaticClass();
	}
	
	F<PROJECT_NAME>AppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<U<PROJECT_NAME>EquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (U<PROJECT_NAME>AbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const U<PROJECT_NAME>AbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);


	MarkItemDirty(NewEntry);

	return Result;
}

void F<PROJECT_NAME>EquipmentList::RemoveEntry(U<PROJECT_NAME>EquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		F<PROJECT_NAME>AppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (U<PROJECT_NAME>AbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();
			

			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>EquipmentManagerComponent

U<PROJECT_NAME>EquipmentManagerComponent::U<PROJECT_NAME>EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void U<PROJECT_NAME>EquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

U<PROJECT_NAME>EquipmentInstance* U<PROJECT_NAME>EquipmentManagerComponent::EquipItem(TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipmentClass)
{
	U<PROJECT_NAME>EquipmentInstance* Result = nullptr;
	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void U<PROJECT_NAME>EquipmentManagerComponent::UnequipItem(U<PROJECT_NAME>EquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

bool U<PROJECT_NAME>EquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (F<PROJECT_NAME>AppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		U<PROJECT_NAME>EquipmentInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void U<PROJECT_NAME>EquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void U<PROJECT_NAME>EquipmentManagerComponent::UninitializeComponent()
{
	TArray<U<PROJECT_NAME>EquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const F<PROJECT_NAME>AppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (U<PROJECT_NAME>EquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

U<PROJECT_NAME>EquipmentInstance* U<PROJECT_NAME>EquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<U<PROJECT_NAME>EquipmentInstance> InstanceType)
{
	for (F<PROJECT_NAME>AppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (U<PROJECT_NAME>EquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<U<PROJECT_NAME>EquipmentInstance*> U<PROJECT_NAME>EquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<U<PROJECT_NAME>EquipmentInstance> InstanceType) const
{
	TArray<U<PROJECT_NAME>EquipmentInstance*> Results;
	for (const F<PROJECT_NAME>AppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (U<PROJECT_NAME>EquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

