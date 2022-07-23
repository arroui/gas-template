// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>InventoryManagerComponent.h"
#include "<PROJECT_NAME>InventoryItemInstance.h"
#include "<PROJECT_NAME>InventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_<PROJECT_NAME>_Inventory_Message_StackChanged, "<PROJECT_NAME>.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>InventoryEntry

FString F<PROJECT_NAME>InventoryEntry::GetDebugString() const
{
	TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>InventoryList

void F<PROJECT_NAME>InventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		F<PROJECT_NAME>InventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void F<PROJECT_NAME>InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		F<PROJECT_NAME>InventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void F<PROJECT_NAME>InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		F<PROJECT_NAME>InventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void F<PROJECT_NAME>InventoryList::BroadcastChangeMessage(F<PROJECT_NAME>InventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	F<PROJECT_NAME>InventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_<PROJECT_NAME>_Inventory_Message_StackChanged, Message);
}

U<PROJECT_NAME>InventoryItemInstance* F<PROJECT_NAME>InventoryList::AddEntry(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 StackCount)
{
	U<PROJECT_NAME>InventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	F<PROJECT_NAME>InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<U<PROJECT_NAME>InventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (U<PROJECT_NAME>InventoryItemFragment* Fragment : GetDefault<U<PROJECT_NAME>InventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const U<PROJECT_NAME>InventoryItemDefinition* ItemCDO = GetDefault<U<PROJECT_NAME>InventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void F<PROJECT_NAME>InventoryList::AddEntry(U<PROJECT_NAME>InventoryItemInstance* Instance)
{
	unimplemented();
}

void F<PROJECT_NAME>InventoryList::RemoveEntry(U<PROJECT_NAME>InventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		F<PROJECT_NAME>InventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<U<PROJECT_NAME>InventoryItemInstance*> F<PROJECT_NAME>InventoryList::GetAllItems() const
{
	TArray<U<PROJECT_NAME>InventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const F<PROJECT_NAME>InventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>InventoryManagerComponent

U<PROJECT_NAME>InventoryManagerComponent::U<PROJECT_NAME>InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void U<PROJECT_NAME>InventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool U<PROJECT_NAME>InventoryManagerComponent::CanAddItemDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

U<PROJECT_NAME>InventoryItemInstance* U<PROJECT_NAME>InventoryManagerComponent::AddItemDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 StackCount)
{
	U<PROJECT_NAME>InventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
	}
	return Result;
}

void U<PROJECT_NAME>InventoryManagerComponent::AddItemInstance(U<PROJECT_NAME>InventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
}

void U<PROJECT_NAME>InventoryManagerComponent::RemoveItemInstance(U<PROJECT_NAME>InventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);
}

TArray<U<PROJECT_NAME>InventoryItemInstance*> U<PROJECT_NAME>InventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

U<PROJECT_NAME>InventoryItemInstance* U<PROJECT_NAME>InventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef) const
{
	for (const F<PROJECT_NAME>InventoryEntry& Entry : InventoryList.Entries)
	{
		U<PROJECT_NAME>InventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 U<PROJECT_NAME>InventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const F<PROJECT_NAME>InventoryEntry& Entry : InventoryList.Entries)
	{
		U<PROJECT_NAME>InventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool U<PROJECT_NAME>InventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (U<PROJECT_NAME>InventoryItemInstance* Instance = U<PROJECT_NAME>InventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

bool U<PROJECT_NAME>InventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (F<PROJECT_NAME>InventoryEntry& Entry : InventoryList.Entries)
	{
		U<PROJECT_NAME>InventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class U<PROJECT_NAME>InventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(U<PROJECT_NAME>InventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class U<PROJECT_NAME>InventoryFilter_HasTag : public U<PROJECT_NAME>InventoryFilter
// {
// public:
// 	virtual bool PassesFilter(U<PROJECT_NAME>InventoryItemInstance* Instance) const { return true; }
// };

