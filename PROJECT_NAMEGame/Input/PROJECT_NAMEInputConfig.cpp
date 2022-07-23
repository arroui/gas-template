// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>InputConfig.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "InputMappingContext.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"


U<PROJECT_NAME>InputConfig::U<PROJECT_NAME>InputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* U<PROJECT_NAME>InputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const F<PROJECT_NAME>InputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* U<PROJECT_NAME>InputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const F<PROJECT_NAME>InputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}