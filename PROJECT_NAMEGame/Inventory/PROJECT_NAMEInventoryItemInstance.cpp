// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/<PROJECT_NAME>InventoryItemDefinition.h"

U<PROJECT_NAME>InventoryItemInstance::U<PROJECT_NAME>InventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>InventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void U<PROJECT_NAME>InventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void U<PROJECT_NAME>InventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 U<PROJECT_NAME>InventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool U<PROJECT_NAME>InventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void U<PROJECT_NAME>InventoryItemInstance::SetItemDef(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

const U<PROJECT_NAME>InventoryItemFragment* U<PROJECT_NAME>InventoryItemInstance::FindFragmentByClass(TSubclassOf<U<PROJECT_NAME>InventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<U<PROJECT_NAME>InventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

