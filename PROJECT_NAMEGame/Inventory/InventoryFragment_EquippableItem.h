// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/<PROJECT_NAME>InventoryItemDefinition.h"
#include "Equipment/<PROJECT_NAME>EquipmentDefinition.h"

#include "InventoryFragment_EquippableItem.generated.h"

UCLASS()
class UInventoryFragment_EquippableItem : public U<PROJECT_NAME>InventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=<PROJECT_NAME>)
	TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipmentDefinition;
};
