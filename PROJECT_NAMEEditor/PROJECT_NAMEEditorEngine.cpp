// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>EditorEngine.h"

#include "Development/<PROJECT_NAME>DeveloperSettings.h"
#include "Development/<PROJECT_NAME>PlatformEmulationSettings.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameModes/<PROJECT_NAME>WorldSettings.h"
#include "Settings/ContentBrowserSettings.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Commands/InputBindingManager.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>Editor"

U<PROJECT_NAME>EditorEngine::U<PROJECT_NAME>EditorEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>EditorEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

void U<PROJECT_NAME>EditorEngine::Start()
{
	Super::Start();
}

void U<PROJECT_NAME>EditorEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds, bIdleMode);
	
	FirstTickSetup();
}

void U<PROJECT_NAME>EditorEngine::FirstTickSetup()
{
	if (bFirstTickSetup)
	{
		return;
	}

	bFirstTickSetup = true;

	// Force show plugin content on load.
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(true);

	// Set PIE to default to Shift+Escape if no user overrides have been set.
	{
		FInputChord OutPrimaryChord, OutSecondaryChord;
		FInputBindingManager::Get().GetUserDefinedChord("PlayWorld", "StopPlaySession", EMultipleKeyBindingIndex::Primary, OutPrimaryChord);
		FInputBindingManager::Get().GetUserDefinedChord("PlayWorld", "StopPlaySession", EMultipleKeyBindingIndex::Secondary, OutSecondaryChord);

		// Is there already a user setting for stopping PIE?  Then don't do this.
		if (!(OutPrimaryChord.IsValidChord() || OutSecondaryChord.IsValidChord()))
		{
			TSharedPtr<FUICommandInfo> StopCommand = FInputBindingManager::Get().FindCommandInContext("PlayWorld", "StopPlaySession");
			if (ensure(StopCommand))
			{
				// Shift+Escape to exit PIE.  Some folks like Ctrl+Q, if that's the case, change it here for your team.
				StopCommand->SetActiveChord(FInputChord(EKeys::Escape, true, false, false, false), EMultipleKeyBindingIndex::Primary);
				FInputBindingManager::Get().NotifyActiveChordChanged(*StopCommand, EMultipleKeyBindingIndex::Primary);
			}
		}
	}
}

FGameInstancePIEResult U<PROJECT_NAME>EditorEngine::PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances)
{
	if (const A<PROJECT_NAME>WorldSettings* <PROJECT_NAME>WorldSettings = Cast<A<PROJECT_NAME>WorldSettings>(EditorWorld->GetWorldSettings()))
	{
		if (<PROJECT_NAME>WorldSettings->ForceStandaloneNetMode)
		{
			EPlayNetMode OutPlayNetMode;
			PlaySessionRequest->EditorPlaySettings->GetPlayNetMode(OutPlayNetMode);
			if (OutPlayNetMode != PIE_Standalone)
			{
				PlaySessionRequest->EditorPlaySettings->SetPlayNetMode(PIE_Standalone);

				FNotificationInfo Info(LOCTEXT("ForcingStandaloneForFrontend", "Forcing NetMode: Standalone for the Frontend"));
				Info.ExpireDuration = 2.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
	}

	//@TODO: Should add delegates that a *non-editor* module could bind to for PIE start/stop instead of poking directly
	GetDefault<U<PROJECT_NAME>DeveloperSettings>()->OnPlayInEditorStarted();
	GetDefault<U<PROJECT_NAME>PlatformEmulationSettings>()->OnPlayInEditorStarted();

	//
	FGameInstancePIEResult Result = Super::PreCreatePIEServerInstance(bAnyBlueprintErrors, bStartInSpectatorMode, PIEStartTime, bSupportsOnlinePIE, InNumOnlinePIEInstances);

	return Result;
}

#undef LOCTEXT_NAMESPACE