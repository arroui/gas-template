// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PerformanceStatSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "GameModes/<PROJECT_NAME>GameState.h"

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>PerformanceStatCache

void F<PROJECT_NAME>PerformanceStatCache::StartCharting()
{
}

void F<PROJECT_NAME>PerformanceStatCache::ProcessFrame(const FFrameData& FrameData)
{
	CachedData = FrameData;
	CachedServerFPS = 0.0f;
	CachedPingMS = 0.0f;
	CachedPacketLossIncomingPercent = 0.0f;
	CachedPacketLossOutgoingPercent = 0.0f;
	CachedPacketRateIncoming = 0.0f;
	CachedPacketRateOutgoing = 0.0f;
	CachedPacketSizeIncoming = 0.0f;
	CachedPacketSizeOutgoing = 0.0f;

	if (UWorld* World = MySubsystem->GetGameInstance()->GetWorld())
	{
		if (const A<PROJECT_NAME>GameState* GameState = World->GetGameState<A<PROJECT_NAME>GameState>())
		{
			CachedServerFPS = GameState->GetServerFPS();
		}

		if (APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World))
		{
			if (APlayerState* PS = LocalPC->GetPlayerState<APlayerState>())
			{
				CachedPingMS = PS->GetPingInMilliseconds();
			}

			if (UNetConnection* NetConnection = LocalPC->GetNetConnection())
			{
				const UNetConnection::FNetConnectionPacketLoss& InLoss = NetConnection->GetInLossPercentage();
				CachedPacketLossIncomingPercent = InLoss.GetAvgLossPercentage();
				const UNetConnection::FNetConnectionPacketLoss& OutLoss = NetConnection->GetOutLossPercentage();
				CachedPacketLossOutgoingPercent = OutLoss.GetAvgLossPercentage();

				CachedPacketRateIncoming = NetConnection->InPacketsPerSecond;
				CachedPacketRateOutgoing = NetConnection->OutPacketsPerSecond;

				CachedPacketSizeIncoming = (NetConnection->InPacketsPerSecond != 0) ? (NetConnection->InBytesPerSecond / (float)NetConnection->InPacketsPerSecond) : 0.0f;
				CachedPacketSizeOutgoing = (NetConnection->OutPacketsPerSecond != 0) ? (NetConnection->OutBytesPerSecond / (float)NetConnection->OutPacketsPerSecond) : 0.0f;
			}
		}
	}
}

void F<PROJECT_NAME>PerformanceStatCache::StopCharting()
{
}

double F<PROJECT_NAME>PerformanceStatCache::GetCachedStat(E<PROJECT_NAME>DisplayablePerformanceStat Stat) const
{
	static_assert((int32)E<PROJECT_NAME>DisplayablePerformanceStat::Count == 15, "Need to update this function to deal with new performance stats");
	switch (Stat)
	{
	case E<PROJECT_NAME>DisplayablePerformanceStat::ClientFPS:
		return (CachedData.TrueDeltaSeconds != 0.0) ? (1.0 / CachedData.TrueDeltaSeconds) : 0.0;
	case E<PROJECT_NAME>DisplayablePerformanceStat::ServerFPS:
		return CachedServerFPS;
	case E<PROJECT_NAME>DisplayablePerformanceStat::IdleTime:
		return CachedData.IdleSeconds;
	case E<PROJECT_NAME>DisplayablePerformanceStat::FrameTime:
		return CachedData.TrueDeltaSeconds;
	case E<PROJECT_NAME>DisplayablePerformanceStat::FrameTime_GameThread:
		return CachedData.GameThreadTimeSeconds;
	case E<PROJECT_NAME>DisplayablePerformanceStat::FrameTime_RenderThread:
		return CachedData.RenderThreadTimeSeconds;
	case E<PROJECT_NAME>DisplayablePerformanceStat::FrameTime_RHIThread:
		return CachedData.RHIThreadTimeSeconds;
	case E<PROJECT_NAME>DisplayablePerformanceStat::FrameTime_GPU:
		return CachedData.GPUTimeSeconds;
	case E<PROJECT_NAME>DisplayablePerformanceStat::Ping:
		return CachedPingMS;
	case E<PROJECT_NAME>DisplayablePerformanceStat::PacketLoss_Incoming:
		return CachedPacketLossIncomingPercent;
	case E<PROJECT_NAME>DisplayablePerformanceStat::PacketLoss_Outgoing:
		return CachedPacketLossOutgoingPercent;
	case E<PROJECT_NAME>DisplayablePerformanceStat::PacketRate_Incoming:
		return CachedPacketRateIncoming;
	case E<PROJECT_NAME>DisplayablePerformanceStat::PacketRate_Outgoing:
		return CachedPacketRateOutgoing;
	case E<PROJECT_NAME>DisplayablePerformanceStat::PacketSize_Incoming:
		return CachedPacketSizeIncoming;
	case E<PROJECT_NAME>DisplayablePerformanceStat::PacketSize_Outgoing:
		return CachedPacketSizeOutgoing;
	}

	return 0.0f;
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>PerformanceStatSubsystem

void U<PROJECT_NAME>PerformanceStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Tracker = MakeShared<F<PROJECT_NAME>PerformanceStatCache>(this);
	GEngine->AddPerformanceDataConsumer(Tracker);
}

void U<PROJECT_NAME>PerformanceStatSubsystem::Deinitialize()
{
	GEngine->RemovePerformanceDataConsumer(Tracker);
	Tracker.Reset();
}

double U<PROJECT_NAME>PerformanceStatSubsystem::GetCachedStat(E<PROJECT_NAME>DisplayablePerformanceStat Stat) const
{
	return Tracker->GetCachedStat(Stat);
}
