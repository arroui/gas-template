// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AbilityCost_InventoryItem.h"
#include "<PROJECT_NAME>GameplayAbility.h"
#include "Inventory/<PROJECT_NAME>InventoryManagerComponent.h"

U<PROJECT_NAME>AbilityCost_InventoryItem::U<PROJECT_NAME>AbilityCost_InventoryItem()
{
	Quantity.SetValue(1.0f);
}

bool U<PROJECT_NAME>AbilityCost_InventoryItem::CheckCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
#if 0
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (U<PROJECT_NAME>InventoryManagerComponent* InventoryComponent = PC->GetComponentByClass<U<PROJECT_NAME>InventoryManagerComponent>())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

			return InventoryComponent->GetTotalItemCountByDefinition(ItemDefinition) >= NumItemsToConsume;
		}
	}
#endif
	return false;
}

void U<PROJECT_NAME>AbilityCost_InventoryItem::ApplyCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
#if 0
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (U<PROJECT_NAME>InventoryManagerComponent* InventoryComponent = PC->GetComponentByClass<U<PROJECT_NAME>InventoryManagerComponent>())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

				InventoryComponent->ConsumeItemsByDefinition(ItemDefinition, NumItemsToConsume);
			}
		}
	}
#endif
}
