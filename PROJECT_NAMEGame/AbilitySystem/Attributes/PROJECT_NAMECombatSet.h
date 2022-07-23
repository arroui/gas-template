// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "<PROJECT_NAME>AttributeSet.h"
#include "<PROJECT_NAME>CombatSet.generated.h"


/**
 * U<PROJECT_NAME>CombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class U<PROJECT_NAME>CombatSet : public U<PROJECT_NAME>AttributeSet
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>CombatSet();

	ATTRIBUTE_ACCESSORS(U<PROJECT_NAME>CombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(U<PROJECT_NAME>CombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "<PROJECT_NAME>|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "<PROJECT_NAME>|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
