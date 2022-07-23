// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameplayEffectContext.h"
#include "Components/PrimitiveComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySourceInterface.h"

F<PROJECT_NAME>GameplayEffectContext* F<PROJECT_NAME>GameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(F<PROJECT_NAME>GameplayEffectContext::StaticStruct()))
	{
		return (F<PROJECT_NAME>GameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool F<PROJECT_NAME>GameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

void F<PROJECT_NAME>GameplayEffectContext::SetAbilitySource(const I<PROJECT_NAME>AbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const I<PROJECT_NAME>AbilitySourceInterface* F<PROJECT_NAME>GameplayEffectContext::GetAbilitySource() const
{
	return Cast<I<PROJECT_NAME>AbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* F<PROJECT_NAME>GameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}
