// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/<PROJECT_NAME>InventoryItemDefinition.h"
#include "GameplayTagContainer.h"

#include "InventoryFragment_SetStats.generated.h"

UCLASS()
class UInventoryFragment_SetStats : public U<PROJECT_NAME>InventoryItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(U<PROJECT_NAME>InventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
};
