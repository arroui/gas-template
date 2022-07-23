// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ReplaySubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"

U<PROJECT_NAME>ReplaySubsystem::U<PROJECT_NAME>ReplaySubsystem()
{
}

void U<PROJECT_NAME>ReplaySubsystem::PlayReplay(U<PROJECT_NAME>ReplayListEntry* Replay)
{
	if (Replay != nullptr)
	{
		FString DemoName = Replay->StreamInfo.Name;
		GetGameInstance()->PlayReplay(DemoName);
	}
}

// void U<PROJECT_NAME>ReplaySubsystem::DeleteReplay()
// {
//	ReplayStreamer->DeleteFinishedStream(SelectedItem->StreamInfo.Name, FDeleteFinishedStreamCallback::CreateSP(this, &SShooterDemoList::OnDeleteFinishedStreamComplete));
// }

void U<PROJECT_NAME>ReplaySubsystem::SeekInActiveReplay(float TimeInSeconds)
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		DemoDriver->GotoTimeInSeconds(TimeInSeconds);
	}
}

float U<PROJECT_NAME>ReplaySubsystem::GetReplayLengthInSeconds() const
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		return DemoDriver->GetDemoTotalTime();
	}
	return 0.0f;
}

float U<PROJECT_NAME>ReplaySubsystem::GetReplayCurrentTime() const
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		return DemoDriver->GetDemoCurrentTime();
	}
	return 0.0f;
}

UDemoNetDriver* U<PROJECT_NAME>ReplaySubsystem::GetDemoDriver() const
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		return World->GetDemoNetDriver();
	}
	return nullptr;
}
