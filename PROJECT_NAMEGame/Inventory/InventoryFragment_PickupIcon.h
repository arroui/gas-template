// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/<PROJECT_NAME>InventoryItemDefinition.h"

#include "InventoryFragment_PickupIcon.generated.h"

class USkeletalMesh;

UCLASS()
class UInventoryFragment_PickupIcon : public U<PROJECT_NAME>InventoryItemFragment
{
	GENERATED_BODY()

public:
	UInventoryFragment_PickupIcon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FLinearColor PadColor;
};
