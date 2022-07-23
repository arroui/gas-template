// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/<PROJECT_NAME>GameplayAbility.h"
#include "<PROJECT_NAME>GameplayAbility_Reset.generated.h"

/**
 * U<PROJECT_NAME>GameplayAbility_Reset
 *
 *	Gameplay ability used for handling quickly resetting the player back to initial spawn state.
 *	Ability is activated automatically via the "GameplayEvent.RequestReset" ability trigger tag (server only).
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>GameplayAbility_Reset : public U<PROJECT_NAME>GameplayAbility
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>GameplayAbility_Reset(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	
	void DoneAddingNativeTags();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};



USTRUCT(BlueprintType)
struct F<PROJECT_NAME>PlayerResetMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* OwnerPlayerState = nullptr;
};