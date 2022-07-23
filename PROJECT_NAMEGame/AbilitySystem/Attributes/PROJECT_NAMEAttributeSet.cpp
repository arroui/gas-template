// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AttributeSet.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"


U<PROJECT_NAME>AttributeSet::U<PROJECT_NAME>AttributeSet()
{
}

UWorld* U<PROJECT_NAME>AttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

U<PROJECT_NAME>AbilitySystemComponent* U<PROJECT_NAME>AttributeSet::Get<PROJECT_NAME>AbilitySystemComponent() const
{
	return Cast<U<PROJECT_NAME>AbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
