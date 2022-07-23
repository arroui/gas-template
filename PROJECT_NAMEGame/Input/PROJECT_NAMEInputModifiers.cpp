// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>InputModifiers.h"
#include "Settings/<PROJECT_NAME>SettingsShared.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Input/<PROJECT_NAME>AimSensitivityData.h"

DEFINE_LOG_CATEGORY_STATIC(Log<PROJECT_NAME>InputModifiers, Log, All);

//////////////////////////////////////////////////////////////////////
// <PROJECT_NAME>InputModifiersHelpers

namespace <PROJECT_NAME>InputModifiersHelpers
{
	/** Returns the owning <PROJECT_NAME>LocalPlayer of an Enhanced Player Input pointer */
	static U<PROJECT_NAME>LocalPlayer* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		if (PlayerInput)
		{
			if (APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
			{
				return Cast<U<PROJECT_NAME>LocalPlayer>(PC->GetLocalPlayer());
			}
		}
		return nullptr;
	}
	
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>SettingBasedScalar

FInputActionValue U<PROJECT_NAME>SettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean, TEXT("Setting Based Scalar modifier doesn't support boolean values.")))
	{
		if (U<PROJECT_NAME>LocalPlayer* LocalPlayer = <PROJECT_NAME>InputModifiersHelpers::GetLocalPlayer(PlayerInput))
		{
			const UClass* SettingsClass = U<PROJECT_NAME>SettingsShared::StaticClass();
			U<PROJECT_NAME>SettingsShared* SharedSettings = LocalPlayer->GetSharedSettings();
			
			const bool bHasCachedProperty = PropertyCache.Num() == 3;
			
			const FProperty* XAxisValue = bHasCachedProperty ? PropertyCache[0] : SettingsClass->FindPropertyByName(XAxisScalarSettingName);
			const FProperty* YAxisValue = bHasCachedProperty ? PropertyCache[1] : SettingsClass->FindPropertyByName(YAxisScalarSettingName);
			const FProperty* ZAxisValue = bHasCachedProperty ? PropertyCache[2] : SettingsClass->FindPropertyByName(ZAxisScalarSettingName);

			if (PropertyCache.IsEmpty())
			{
				PropertyCache.Emplace(XAxisValue);
				PropertyCache.Emplace(YAxisValue);
				PropertyCache.Emplace(ZAxisValue);
			}

			FVector ScalarToUse = FVector(1.0, 1.0, 1.0);

			switch (CurrentValue.GetValueType())
			{
			case EInputActionValueType::Axis3D:
				ScalarToUse.Z = ZAxisValue ? *ZAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				//[[fallthrough]];
			case EInputActionValueType::Axis2D:
				ScalarToUse.Y = YAxisValue ? *YAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				//[[fallthrough]];
			case EInputActionValueType::Axis1D:
				ScalarToUse.X = XAxisValue ? *XAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				break;
			}

			ScalarToUse.X = FMath::Clamp(ScalarToUse.X, MinValueClamp.X, MaxValueClamp.X);
			ScalarToUse.Y = FMath::Clamp(ScalarToUse.Y, MinValueClamp.Y, MaxValueClamp.Y);
			ScalarToUse.Z = FMath::Clamp(ScalarToUse.Z, MinValueClamp.Z, MaxValueClamp.Z);
			
			return CurrentValue.Get<FVector>() * ScalarToUse;
		}
	}
	
	return CurrentValue;	
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>InputModifierDeadZone

FInputActionValue U<PROJECT_NAME>InputModifierDeadZone::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	EInputActionValueType ValueType = CurrentValue.GetValueType();
	U<PROJECT_NAME>LocalPlayer* LocalPlayer = <PROJECT_NAME>InputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (ValueType == EInputActionValueType::Boolean || !LocalPlayer)
	{
		return CurrentValue;
	}
	
	U<PROJECT_NAME>SettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	float LowerThreshold =
		(DeadzoneStick == EDeadzoneStick::MoveStick) ? 
		Settings->GetGamepadMoveStickDeadZone() :
		Settings->GetGamepadLookStickDeadZone();
	
	LowerThreshold = FMath::Clamp(LowerThreshold, 0.0f, 1.0f);
	
	auto DeadZoneLambda = [LowerThreshold, this](const float AxisVal)
	{
		// We need to translate and scale the input to the +/- 1 range after removing the dead zone.
		return FMath::Min(1.f, (FMath::Max(0.f, FMath::Abs(AxisVal) - LowerThreshold) / (UpperThreshold - LowerThreshold))) * FMath::Sign(AxisVal);
	};

	FVector NewValue = CurrentValue.Get<FVector>();
	switch (Type)
	{
	case EDeadZoneType::Axial:
		NewValue.X = DeadZoneLambda(NewValue.X);
		NewValue.Y = DeadZoneLambda(NewValue.Y);
		NewValue.Z = DeadZoneLambda(NewValue.Z);
		break;
	case EDeadZoneType::Radial:
		if (ValueType == EInputActionValueType::Axis3D)
		{
			NewValue = NewValue.GetSafeNormal() * DeadZoneLambda(NewValue.Size());
		}
		else if (ValueType == EInputActionValueType::Axis2D)
		{
			NewValue = NewValue.GetSafeNormal2D() * DeadZoneLambda(NewValue.Size2D());
		}
		else
		{
			NewValue.X = DeadZoneLambda(NewValue.X);
		}
		break;
	}

	return NewValue;
}

FLinearColor U<PROJECT_NAME>InputModifierDeadZone::GetVisualizationColor_Implementation(FInputActionValue SampleValue, FInputActionValue FinalValue) const
{
	// Taken from UInputModifierDeadZone::GetVisualizationColor_Implementation
	if (FinalValue.GetValueType() == EInputActionValueType::Boolean || FinalValue.GetValueType() == EInputActionValueType::Axis1D)
	{
		return FLinearColor(FinalValue.Get<float>() == 0.f ? 1.f : 0.f, 0.f, 0.f);
	}
	return FLinearColor((FinalValue.Get<FVector2D>().X == 0.f ? 0.5f : 0.f) + (FinalValue.Get<FVector2D>().Y == 0.f ? 0.5f : 0.f), 0.f, 0.f);
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>InputModifierGamepadSensitivity

FInputActionValue U<PROJECT_NAME>InputModifierGamepadSensitivity::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// You can't scale a boolean action type
	U<PROJECT_NAME>LocalPlayer* LocalPlayer = <PROJECT_NAME>InputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (CurrentValue.GetValueType() == EInputActionValueType::Boolean || !LocalPlayer || !SensitivityLevelTable)
	{
		return CurrentValue;
	}
	
	U<PROJECT_NAME>SettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	const E<PROJECT_NAME>GamepadSensitivity Sensitivity = (TargetingType == E<PROJECT_NAME>TargetingType::Normal) ? Settings->GetGamepadLookSensitivityPreset() : Settings->GetGamepadTargetingSensitivityPreset();

	const float Scalar = SensitivityLevelTable->SensitivtyEnumToFloat(Sensitivity);

	return CurrentValue.Get<FVector>() * Scalar;
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>InputModifierAimInversion

FInputActionValue U<PROJECT_NAME>InputModifierAimInversion::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	U<PROJECT_NAME>LocalPlayer* LocalPlayer = <PROJECT_NAME>InputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (!LocalPlayer)
	{
		return CurrentValue;
	}
	
	U<PROJECT_NAME>SettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();
	
	if (Settings->GetInvertVerticalAxis())
	{
		NewValue.Y *= -1.0f;
	}
	
	if (Settings->GetInvertHorizontalAxis())
	{
		NewValue.X *= -1.0f;
	}
	
	return NewValue;
}