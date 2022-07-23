// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "<PROJECT_NAME>HealExecution.generated.h"


/**
 * U<PROJECT_NAME>HealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class U<PROJECT_NAME>HealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>HealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
