// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AbilityCost_ItemTagStack.h"

#include "Equipment/<PROJECT_NAME>EquipmentInstance.h"
#include "Inventory/<PROJECT_NAME>InventoryItemInstance.h"
#include "Equipment/<PROJECT_NAME>GameplayAbility_FromEquipment.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost");

U<PROJECT_NAME>AbilityCost_ItemTagStack::U<PROJECT_NAME>AbilityCost_ItemTagStack()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool U<PROJECT_NAME>AbilityCost_ItemTagStack::CheckCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (const U<PROJECT_NAME>GameplayAbility_FromEquipment* EquipmentAbility = Cast<const U<PROJECT_NAME>GameplayAbility_FromEquipment>(Ability))
	{
		if (U<PROJECT_NAME>InventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

			// Inform other abilities why this cost cannot be applied
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);				
			}
			return bCanApplyCost;
		}
	}
	return false;
}

void U<PROJECT_NAME>AbilityCost_ItemTagStack::ApplyCost(const U<PROJECT_NAME>GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (const U<PROJECT_NAME>GameplayAbility_FromEquipment* EquipmentAbility = Cast<const U<PROJECT_NAME>GameplayAbility_FromEquipment>(Ability))
		{
			if (U<PROJECT_NAME>InventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				ItemInstance->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}
