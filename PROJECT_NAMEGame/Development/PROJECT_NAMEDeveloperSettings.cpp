// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>DeveloperSettings.h"
#include "Misc/App.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>Cheats"

U<PROJECT_NAME>DeveloperSettings::U<PROJECT_NAME>DeveloperSettings()
{
}

FName U<PROJECT_NAME>DeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#if WITH_EDITOR
void U<PROJECT_NAME>DeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplySettings();
}

void U<PROJECT_NAME>DeveloperSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);

	ApplySettings();
}

void U<PROJECT_NAME>DeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();

	ApplySettings();
}

void U<PROJECT_NAME>DeveloperSettings::ApplySettings()
{
}

void U<PROJECT_NAME>DeveloperSettings::OnPlayInEditorStarted() const
{
	// Show a notification toast to remind the user that there's an experience override set
	if (ExperienceOverride.IsValid())
	{
		FNotificationInfo Info(FText::Format(
			LOCTEXT("ExperienceOverrideActive", "Developer Settings Override\nExperience {0}"),
			FText::FromName(ExperienceOverride.PrimaryAssetName)
		));
		Info.ExpireDuration = 2.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}
#endif

#undef LOCTEXT_NAMESPACE
