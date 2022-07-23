// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>InputComponent.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "PlayerMappableInputConfig.h"

U<PROJECT_NAME>InputComponent::U<PROJECT_NAME>InputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void U<PROJECT_NAME>InputComponent::AddInputMappings(const U<PROJECT_NAME>InputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	U<PROJECT_NAME>LocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<U<PROJECT_NAME>LocalPlayer>();
	check(LocalPlayer);

	// Add any registered input mappings from the settings!
	if (U<PROJECT_NAME>SettingsLocal* LocalSettings = U<PROJECT_NAME>SettingsLocal::Get())
	{
		// We don't want to ignore keys that were "Down" when we add the mapping context
		// This allows you to die holding a movement key, keep holding while waiting for respawn,
		// and have it be applied after you respawn immediately. Leaving bIgnoreAllPressedKeysUntilRelease
		// to it's default "true" state would require the player to release the movement key,
		// and press it again when they respawn
		FModifyContextOptions Options = {};
		Options.bIgnoreAllPressedKeysUntilRelease = false;
		
		// Add all registered configs, which will add every input mapping context that is in it
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			if (Pair.bIsActive)
			{
				InputSubsystem->AddPlayerMappableConfig(Pair.Config, Options);	
			}
		}
		
		// Tell enhanced input about any custom keymappings that we have set
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
			}
		}
	}
}

void U<PROJECT_NAME>InputComponent::RemoveInputMappings(const U<PROJECT_NAME>InputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	U<PROJECT_NAME>LocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<U<PROJECT_NAME>LocalPlayer>();
	check(LocalPlayer);
	
	if (U<PROJECT_NAME>SettingsLocal* LocalSettings = U<PROJECT_NAME>SettingsLocal::Get())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}
		
		// Clear any player mapped keys from enhanced input
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			InputSubsystem->RemovePlayerMappedKey(Pair.Key);
		}
	}
}

void U<PROJECT_NAME>InputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}

void U<PROJECT_NAME>InputComponent::AddInputConfig(const FLoadedMappableConfigPair& ConfigPair, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	check(InputSubsystem);
	if (ensure(ConfigPair.bIsActive))
	{
		InputSubsystem->AddPlayerMappableConfig(ConfigPair.Config);	
	}
}

void U<PROJECT_NAME>InputComponent::RemoveInputConfig(const FLoadedMappableConfigPair& ConfigPair, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	check(InputSubsystem);
	if (!ConfigPair.bIsActive)
	{
		InputSubsystem->AddPlayerMappableConfig(ConfigPair.Config);	
	}	
}