// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AbilityCost_PlayerTagStack.h"
#include "Player/<PROJECT_NAME>PlayerState.h"
#include "<PROJECT_NAME>GameplayAbility.h"
#include "GameFramework/PlayerController.h"

U<PROJECT_NAME>AbilityCost_PlayerTagStack::U<PROJECT_NAME>AbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool U<PROJECT_NAME>AbilityCost_PlayerTagStack::CheckCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (A<PROJECT_NAME>PlayerState* PS = Cast<A<PROJECT_NAME>PlayerState>(PC->PlayerState))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			return PS->GetStatTagStackCount(Tag) >= NumStacks;
		}
	}
	return false;
}

void U<PROJECT_NAME>AbilityCost_PlayerTagStack::ApplyCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (A<PROJECT_NAME>PlayerState* PS = Cast<A<PROJECT_NAME>PlayerState>(PC->PlayerState))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				PS->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}
