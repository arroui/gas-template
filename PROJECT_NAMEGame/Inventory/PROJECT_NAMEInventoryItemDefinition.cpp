// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>InventoryItemDefinition.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>InventoryItemDefinition

U<PROJECT_NAME>InventoryItemDefinition::U<PROJECT_NAME>InventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const U<PROJECT_NAME>InventoryItemFragment* U<PROJECT_NAME>InventoryItemDefinition::FindFragmentByClass(TSubclassOf<U<PROJECT_NAME>InventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (U<PROJECT_NAME>InventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>InventoryItemDefinition

const U<PROJECT_NAME>InventoryItemFragment* U<PROJECT_NAME>InventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, TSubclassOf<U<PROJECT_NAME>InventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<U<PROJECT_NAME>InventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
