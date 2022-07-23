// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameplayAbility_FromEquipment.h"
#include "<PROJECT_NAME>EquipmentInstance.h"
#include "Inventory/<PROJECT_NAME>InventoryItemInstance.h"

U<PROJECT_NAME>GameplayAbility_FromEquipment::U<PROJECT_NAME>GameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

U<PROJECT_NAME>EquipmentInstance* U<PROJECT_NAME>GameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<U<PROJECT_NAME>EquipmentInstance>(Spec->SourceObject);
	}

	return nullptr;
}

U<PROJECT_NAME>InventoryItemInstance* U<PROJECT_NAME>GameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (U<PROJECT_NAME>EquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<U<PROJECT_NAME>InventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult U<PROJECT_NAME>GameplayAbility_FromEquipment::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		ValidationErrors.Add(NSLOCTEXT("<PROJECT_NAME>", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif