// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "<PROJECT_NAME>GameplayAbility.h"
#include "<PROJECT_NAME>GameplayAbility_Jump.generated.h"


/**
 * U<PROJECT_NAME>GameplayAbility_Jump
 *
 *	Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class U<PROJECT_NAME>GameplayAbility_Jump : public U<PROJECT_NAME>GameplayAbility
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>GameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Ability")
	void CharacterJumpStop();
};
