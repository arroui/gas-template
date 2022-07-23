// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>WeaponInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

U<PROJECT_NAME>WeaponInstance::U<PROJECT_NAME>WeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>WeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	UWorld* World = GetWorld();
	check(World);
	TimeLastEquipped = World->GetTimeSeconds();
}

void U<PROJECT_NAME>WeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void U<PROJECT_NAME>WeaponInstance::UpdateFiringTime()
{
	UWorld* World = GetWorld();
	check(World);
	TimeLastFired = World->GetTimeSeconds();
}

float U<PROJECT_NAME>WeaponInstance::GetTimeSinceLastInteractedWith() const
{
	UWorld* World = GetWorld();
	check(World);
	const double WorldTime = World->GetTimeSeconds();

	double Result = WorldTime - TimeLastEquipped;

	if (TimeLastFired > 0.0)
	{
		const double TimeSinceFired = WorldTime - TimeLastFired;
		Result = FMath::Min(Result, TimeSinceFired);
	}

	return Result;
}

TSubclassOf<UAnimInstance> U<PROJECT_NAME>WeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
{
	const F<PROJECT_NAME>AnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UneuippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}
