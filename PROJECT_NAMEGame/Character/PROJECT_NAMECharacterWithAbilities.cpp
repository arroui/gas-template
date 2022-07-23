// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>CharacterWithAbilities.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySet.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>HealthSet.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>CombatSet.h"

A<PROJECT_NAME>CharacterWithAbilities::A<PROJECT_NAME>CharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<U<PROJECT_NAME>AbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<U<PROJECT_NAME>HealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<U<PROJECT_NAME>CombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void A<PROJECT_NAME>CharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* A<PROJECT_NAME>CharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
