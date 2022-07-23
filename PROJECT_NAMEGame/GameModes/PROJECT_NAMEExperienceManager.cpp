// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ExperienceManager.h"
#include "Engine/AssetManager.h"
#include "<PROJECT_NAME>ExperienceDefinition.h"
#include "GameModes/<PROJECT_NAME>ExperienceManager.h"
#include "Engine/Engine.h"

#if WITH_EDITOR

void U<PROJECT_NAME>ExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void U<PROJECT_NAME>ExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		U<PROJECT_NAME>ExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<U<PROJECT_NAME>ExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool U<PROJECT_NAME>ExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		U<PROJECT_NAME>ExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<U<PROJECT_NAME>ExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
}

#endif