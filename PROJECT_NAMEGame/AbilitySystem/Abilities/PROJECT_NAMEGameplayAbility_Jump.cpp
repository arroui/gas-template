// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameplayAbility_Jump.h"
#include "Character/<PROJECT_NAME>Character.h"


U<PROJECT_NAME>GameplayAbility_Jump::U<PROJECT_NAME>GameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool U<PROJECT_NAME>GameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const A<PROJECT_NAME>Character* <PROJECT_NAME>Character = Cast<A<PROJECT_NAME>Character>(ActorInfo->AvatarActor.Get());
	if (!<PROJECT_NAME>Character || !<PROJECT_NAME>Character->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void U<PROJECT_NAME>GameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void U<PROJECT_NAME>GameplayAbility_Jump::CharacterJumpStart()
{
	if (A<PROJECT_NAME>Character* <PROJECT_NAME>Character = Get<PROJECT_NAME>CharacterFromActorInfo())
	{
		if (<PROJECT_NAME>Character->IsLocallyControlled() && !<PROJECT_NAME>Character->bPressedJump)
		{
			<PROJECT_NAME>Character->UnCrouch();
			<PROJECT_NAME>Character->Jump();
		}
	}
}

void U<PROJECT_NAME>GameplayAbility_Jump::CharacterJumpStop()
{
	if (A<PROJECT_NAME>Character* <PROJECT_NAME>Character = Get<PROJECT_NAME>CharacterFromActorInfo())
	{
		if (<PROJECT_NAME>Character->IsLocallyControlled() && <PROJECT_NAME>Character->bPressedJump)
		{
			<PROJECT_NAME>Character->StopJumping();
		}
	}
}
