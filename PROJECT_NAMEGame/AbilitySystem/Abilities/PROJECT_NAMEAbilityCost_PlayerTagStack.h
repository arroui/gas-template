// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "<PROJECT_NAME>AbilityCost.h"
#include "<PROJECT_NAME>AbilityCost_PlayerTagStack.generated.h"

/**
 * Represents a cost that requires expending a quantity of a tag stack on the player state
 */
UCLASS(meta=(DisplayName="Player Tag Stack"))
class U<PROJECT_NAME>AbilityCost_PlayerTagStack : public U<PROJECT_NAME>AbilityCost
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>AbilityCost_PlayerTagStack();

	//~U<PROJECT_NAME>AbilityCost interface
	virtual bool CheckCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of U<PROJECT_NAME>AbilityCost interface

protected:
	/** How much of the tag to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FScalableFloat Quantity;

	/** Which tag to spend some of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FGameplayTag Tag;
};
