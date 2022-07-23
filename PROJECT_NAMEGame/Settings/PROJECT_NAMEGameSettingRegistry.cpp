// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameSettingRegistry.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"

#include "GameSetting.h"
#include "GameSettingCollection.h"
#include "<PROJECT_NAME>SettingsLocal.h"
#include "<PROJECT_NAME>SettingsShared.h"

DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>GameSettingRegistry);

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>"

//--------------------------------------
// U<PROJECT_NAME>GameSettingRegistry
//--------------------------------------

U<PROJECT_NAME>GameSettingRegistry::U<PROJECT_NAME>GameSettingRegistry()
{
}

U<PROJECT_NAME>GameSettingRegistry* U<PROJECT_NAME>GameSettingRegistry::Get(U<PROJECT_NAME>LocalPlayer* InLocalPlayer)
{
	U<PROJECT_NAME>GameSettingRegistry* Registry = FindObject<U<PROJECT_NAME>GameSettingRegistry>(InLocalPlayer, TEXT("<PROJECT_NAME>GameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<U<PROJECT_NAME>GameSettingRegistry>(InLocalPlayer, TEXT("<PROJECT_NAME>GameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool U<PROJECT_NAME>GameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (U<PROJECT_NAME>LocalPlayer* LocalPlayer = Cast<U<PROJECT_NAME>LocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void U<PROJECT_NAME>GameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	U<PROJECT_NAME>LocalPlayer* <PROJECT_NAME>LocalPlayer = Cast<U<PROJECT_NAME>LocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(<PROJECT_NAME>LocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, <PROJECT_NAME>LocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(<PROJECT_NAME>LocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(<PROJECT_NAME>LocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(<PROJECT_NAME>LocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(<PROJECT_NAME>LocalPlayer);
	RegisterSetting(GamepadSettings);
}

void U<PROJECT_NAME>GameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (U<PROJECT_NAME>LocalPlayer* LocalPlayer = Cast<U<PROJECT_NAME>LocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE
