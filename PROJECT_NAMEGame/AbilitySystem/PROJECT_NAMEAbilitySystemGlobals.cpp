// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AbilitySystemGlobals.h"
#include "<PROJECT_NAME>GameplayEffectContext.h"

U<PROJECT_NAME>AbilitySystemGlobals::U<PROJECT_NAME>AbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* U<PROJECT_NAME>AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new F<PROJECT_NAME>GameplayEffectContext();
}
