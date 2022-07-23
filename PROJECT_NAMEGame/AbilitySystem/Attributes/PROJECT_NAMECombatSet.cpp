// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>CombatSet.h"
#include "Net/UnrealNetwork.h"


U<PROJECT_NAME>CombatSet::U<PROJECT_NAME>CombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void U<PROJECT_NAME>CombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(U<PROJECT_NAME>CombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(U<PROJECT_NAME>CombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void U<PROJECT_NAME>CombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(U<PROJECT_NAME>CombatSet, BaseDamage, OldValue);
}

void U<PROJECT_NAME>CombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(U<PROJECT_NAME>CombatSet, BaseHeal, OldValue);
}
