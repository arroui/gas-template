// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsyncAction_ExperienceReady.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "GameModes/<PROJECT_NAME>ExperienceManagerComponent.h"

UAsyncAction_ExperienceReady::UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* InWorldContextObject)
{
	UAsyncAction_ExperienceReady* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_ExperienceReady>();
		Action->WorldPtr = World;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncAction_ExperienceReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToExperienceLoading(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step1_HandleGameStateSet);
		}
	}
	else
	{
		// No world so we'll never finish naturally
		SetReadyToDestroy();
	}
}

void UAsyncAction_ExperienceReady::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToExperienceLoading(GameState);
}

void UAsyncAction_ExperienceReady::Step2_ListenToExperienceLoading(AGameStateBase* GameState)
{
	check(GameState);
	U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		UWorld* World = GameState->GetWorld();
		check(World);

		// The experience happened to be already loaded, but still delay a frame to
		// make sure people don't write stuff that relies on this always being true
		//@TODO: Consider not delaying for dynamically spawned stuff / any time after the loading screen has dropped?
		//@TODO: Maybe just inject a random 0-1s delay in the experience load itself?
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady));
	}
	else
	{
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOn<PROJECT_NAME>ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
	}
}

void UAsyncAction_ExperienceReady::Step3_HandleExperienceLoaded(const U<PROJECT_NAME>ExperienceDefinition* CurrentExperience)
{
	Step4_BroadcastReady();
}

void UAsyncAction_ExperienceReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();

	SetReadyToDestroy();
}
