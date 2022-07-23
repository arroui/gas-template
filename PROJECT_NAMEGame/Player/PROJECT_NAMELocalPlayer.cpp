// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>LocalPlayer.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "Settings/<PROJECT_NAME>SettingsShared.h"
#include "Input/<PROJECT_NAME>InputConfig.h"
#include "InputMappingContext.h"
#include "AudioMixerBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"

U<PROJECT_NAME>LocalPlayer::U<PROJECT_NAME>LocalPlayer()
{
}

void U<PROJECT_NAME>LocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (U<PROJECT_NAME>SettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &U<PROJECT_NAME>LocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void U<PROJECT_NAME>LocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

	OnPlayerControllerChanged(PlayerController);
}

bool U<PROJECT_NAME>LocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	OnPlayerControllerChanged(PlayerController);

	return bResult;
}

void U<PROJECT_NAME>LocalPlayer::InitOnlineSession()
{
	OnPlayerControllerChanged(PlayerController);

	Super::InitOnlineSession();
}

void U<PROJECT_NAME>LocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{
	// Stop listening for changes from the old controller
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(LastBoundPC.Get()))
	{
		OldTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	// Grab the current team ID and listen for future changes
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(NewController))
	{
		NewTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
		LastBoundPC = NewController;
	}

	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
}

void U<PROJECT_NAME>LocalPlayer::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	// Do nothing, we merely observe the team of our associated player controller
}

FGenericTeamId U<PROJECT_NAME>LocalPlayer::GetGenericTeamId() const
{
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(PlayerController))
	{
		return ControllerAsTeamProvider->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
}

FOn<PROJECT_NAME>TeamIndexChangedDelegate* U<PROJECT_NAME>LocalPlayer::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

U<PROJECT_NAME>SettingsLocal* U<PROJECT_NAME>LocalPlayer::GetLocalSettings() const
{
	return U<PROJECT_NAME>SettingsLocal::Get();
}

U<PROJECT_NAME>SettingsShared* U<PROJECT_NAME>LocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		SharedSettings = U<PROJECT_NAME>SettingsShared::LoadOrCreateSettings(this);
	}

	return SharedSettings;
}

void U<PROJECT_NAME>LocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void U<PROJECT_NAME>LocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}

void U<PROJECT_NAME>LocalPlayer::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}
