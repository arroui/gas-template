// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/<PROJECT_NAME>InventoryItemDefinition.h"
#include "UI/Weapons/<PROJECT_NAME>ReticleWidgetBase.h"

#include "InventoryFragment_ReticleConfig.generated.h"

UCLASS()
class UInventoryFragment_ReticleConfig : public U<PROJECT_NAME>InventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<U<PROJECT_NAME>ReticleWidgetBase>> ReticleWidgets;
};
