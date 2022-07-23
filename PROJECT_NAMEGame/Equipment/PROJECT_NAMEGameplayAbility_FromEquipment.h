// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/<PROJECT_NAME>GameplayAbility.h"

#include "<PROJECT_NAME>GameplayAbility_FromEquipment.generated.h"

class U<PROJECT_NAME>EquipmentInstance;
class U<PROJECT_NAME>InventoryItemInstance;

/**
 * U<PROJECT_NAME>GameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class U<PROJECT_NAME>GameplayAbility_FromEquipment : public U<PROJECT_NAME>GameplayAbility
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>GameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="<PROJECT_NAME>|Ability")
	U<PROJECT_NAME>EquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Ability")
	U<PROJECT_NAME>InventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif

};
