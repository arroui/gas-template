// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Settings/<PROJECT_NAME>SettingsShared.h"

#include "<PROJECT_NAME>AimSensitivityData.generated.h"

/** Defines a set of gamepad sensitivity to a float value. */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "<PROJECT_NAME> Aim Sensitivity Data", ShortTooltip = "Data asset used to define a map of Gamepad Sensitivty to a float value."))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>AimSensitivityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>AimSensitivityData(const FObjectInitializer& ObjectInitializer);
	
	const float SensitivtyEnumToFloat(const E<PROJECT_NAME>GamepadSensitivity InSensitivity) const;
	
protected:
	/** Map of SensitivityMap settings to their corresponding float */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<E<PROJECT_NAME>GamepadSensitivity, float> SensitivityMap;
};