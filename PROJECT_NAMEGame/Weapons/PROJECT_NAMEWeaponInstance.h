// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/<PROJECT_NAME>EquipmentInstance.h"
#include "Cosmetics/<PROJECT_NAME>CosmeticAnimationTypes.h"
#include "<PROJECT_NAME>WeaponInstance.generated.h"

/**
 * U<PROJECT_NAME>WeaponInstance
 *
 * A piece of equipment representing a weapon spawned and applied to a pawn
 */
UCLASS()
class U<PROJECT_NAME>WeaponInstance : public U<PROJECT_NAME>EquipmentInstance
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>WeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~U<PROJECT_NAME>EquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of U<PROJECT_NAME>EquipmentInstance interface

	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	// Returns how long it's been since the weapon was interacted with (fired or equipped)
	UFUNCTION(BlueprintPure)
	float GetTimeSinceLastInteractedWith() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	F<PROJECT_NAME>AnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	F<PROJECT_NAME>AnimLayerSelectionSet UneuippedAnimSet;

	// Choose the best layer from EquippedAnimSet or UneuippedAnimSet based on the specified gameplay tags
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

private:
	double TimeLastEquipped = 0.0;
	double TimeLastFired = 0.0;
};
