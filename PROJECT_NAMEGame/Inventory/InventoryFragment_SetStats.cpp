// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragment_SetStats.h"
#include "Inventory/<PROJECT_NAME>InventoryItemInstance.h"

void UInventoryFragment_SetStats::OnInstanceCreated(U<PROJECT_NAME>InventoryItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}
}

int32 UInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}