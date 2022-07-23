// Copyright Epic Games, Inc. All Rights Reserved.

#include "Input/<PROJECT_NAME>MappableConfigPair.h"
#include "System/<PROJECT_NAME>AssetManager.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "ICommonUIModule.h"
#include "CommonUISettings.h"
#include "PlayerMappableInputConfig.h"

bool FMappableConfigPair::CanBeActivated() const
{
	const FGameplayTagContainer& PlatformTraits = ICommonUIModule::GetSettings().GetPlatformTraits();

	// If the current platform does NOT have all the dependent traits, then don't activate it
	if (!DependentPlatformTraits.IsEmpty() && !PlatformTraits.HasAll(DependentPlatformTraits))
	{
		return false;
	}

	// If the platform has any of the excluded traits, then we shouldn't activate this config.
	if (!ExcludedPlatformTraits.IsEmpty() && PlatformTraits.HasAny(ExcludedPlatformTraits))
	{
		return false;
	}

	return true;
}

bool FMappableConfigPair::RegisterPair(const FMappableConfigPair& Pair)
{
	U<PROJECT_NAME>AssetManager& AssetManager = U<PROJECT_NAME>AssetManager::Get();

	if (U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get())
	{
		// Register the pair with the settings, but do not activate it yet
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->RegisterInputConfig(Pair.Type, LoadedConfig, false);
			return true;
		}	
	}
	
	return false;
}

bool FMappableConfigPair::ActivatePair(const FMappableConfigPair& Pair)
{
	U<PROJECT_NAME>AssetManager& AssetManager = U<PROJECT_NAME>AssetManager::Get();
	// Only activate a pair that has been successfully registered
	if (FMappableConfigPair::RegisterPair(Pair) && Pair.CanBeActivated())
	{		
		if (U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get())
		{
			if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
			{
				Settings->ActivateInputConfig(LoadedConfig);
				return true;
			}			
		}
	}
	return false;
}

void FMappableConfigPair::DeactivatePair(const FMappableConfigPair& Pair)
{
	U<PROJECT_NAME>AssetManager& AssetManager = U<PROJECT_NAME>AssetManager::Get();
	
	if (U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->DeactivateInputConfig(LoadedConfig);
		}
	}
}

void FMappableConfigPair::UnregisterPair(const FMappableConfigPair& Pair)
{
	U<PROJECT_NAME>AssetManager& AssetManager = U<PROJECT_NAME>AssetManager::Get();

	if (U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->UnregisterInputConfig(LoadedConfig);
		}
	}
}
