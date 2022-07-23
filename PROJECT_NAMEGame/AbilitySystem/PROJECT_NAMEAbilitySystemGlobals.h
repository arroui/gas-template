// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AbilitySystemGlobals.h"
#include "<PROJECT_NAME>AbilitySystemGlobals.generated.h"

class UAbilitySystemComponent;

UCLASS(Config=Game)
class U<PROJECT_NAME>AbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()

	//~UAbilitySystemGlobals interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~End of UAbilitySystemGlobals interface
};
