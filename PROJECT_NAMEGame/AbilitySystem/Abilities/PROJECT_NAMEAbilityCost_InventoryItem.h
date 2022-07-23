// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "<PROJECT_NAME>AbilityCost.h"
#include "<PROJECT_NAME>AbilityCost_InventoryItem.generated.h"

class U<PROJECT_NAME>InventoryItemDefinition;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class U<PROJECT_NAME>AbilityCost_InventoryItem : public U<PROJECT_NAME>AbilityCost
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>AbilityCost_InventoryItem();

	//~U<PROJECT_NAME>AbilityCost interface
	virtual bool CheckCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of U<PROJECT_NAME>AbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDefinition;
};
