// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>HealExecution.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>HealthSet.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>CombatSet.h"


struct FHealStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealDef;

	FHealStatics()
	{
		BaseHealDef = FGameplayEffectAttributeCaptureDefinition(U<PROJECT_NAME>CombatSet::GetBaseHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealStatics& HealStatics()
{
	static FHealStatics Statics;
	return Statics;
}


U<PROJECT_NAME>HealExecution::U<PROJECT_NAME>HealExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealDef);
}

void U<PROJECT_NAME>HealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHeal = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);

	const float HealingDone = FMath::Max(0.0f, BaseHeal);

	if (HealingDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(U<PROJECT_NAME>HealthSet::GetHealthAttribute(), EGameplayModOp::Additive, HealingDone));
	}
#endif // #if WITH_SERVER_CODE
}
