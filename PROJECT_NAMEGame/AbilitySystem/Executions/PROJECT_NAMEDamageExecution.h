// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "<PROJECT_NAME>DamageExecution.generated.h"


/**
 * U<PROJECT_NAME>DamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class U<PROJECT_NAME>DamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>DamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
