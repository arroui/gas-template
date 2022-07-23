// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameplayAbilityTargetData_SingleTargetHit.h"
#include "<PROJECT_NAME>GameplayEffectContext.h"

//////////////////////////////////////////////////////////////////////

void F<PROJECT_NAME>GameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const
{
	FGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(Context, bIncludeActorArray);

	// Add game-specific data
	if (F<PROJECT_NAME>GameplayEffectContext* TypedContext = F<PROJECT_NAME>GameplayEffectContext::ExtractEffectContext(Context))
	{
		TypedContext->CartridgeID = CartridgeID;
	}
}

bool F<PROJECT_NAME>GameplayAbilityTargetData_SingleTargetHit::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

	Ar << CartridgeID;

	return true;
}
