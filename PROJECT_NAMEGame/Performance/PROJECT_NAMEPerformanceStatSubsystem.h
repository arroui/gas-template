// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ChartCreation.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "<PROJECT_NAME>PerformanceStatTypes.h"

#include "<PROJECT_NAME>PerformanceStatSubsystem.generated.h"

class U<PROJECT_NAME>PerformanceStatSubsystem;

//////////////////////////////////////////////////////////////////////

// Observer which caches the stats for the previous frame
struct F<PROJECT_NAME>PerformanceStatCache : public IPerformanceDataConsumer
{
public:
	F<PROJECT_NAME>PerformanceStatCache(U<PROJECT_NAME>PerformanceStatSubsystem* InSubsystem)
		: MySubsystem(InSubsystem)
	{
	}

	//~IPerformanceDataConsumer interface
	virtual void StartCharting() override;
	virtual void ProcessFrame(const FFrameData& FrameData) override;
	virtual void StopCharting() override;
	//~End of IPerformanceDataConsumer interface

	double GetCachedStat(E<PROJECT_NAME>DisplayablePerformanceStat Stat) const;

protected:
	IPerformanceDataConsumer::FFrameData CachedData;
	U<PROJECT_NAME>PerformanceStatSubsystem* MySubsystem;

	float CachedServerFPS = 0.0f;
	float CachedPingMS = 0.0f;
	float CachedPacketLossIncomingPercent = 0.0f;
	float CachedPacketLossOutgoingPercent = 0.0f;
	float CachedPacketRateIncoming = 0.0f;
	float CachedPacketRateOutgoing = 0.0f;
	float CachedPacketSizeIncoming = 0.0f;
	float CachedPacketSizeOutgoing = 0.0f;
};

//////////////////////////////////////////////////////////////////////

// Subsystem to allow access to performance stats for display purposes
UCLASS(BlueprintType)
class U<PROJECT_NAME>PerformanceStatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	double GetCachedStat(E<PROJECT_NAME>DisplayablePerformanceStat Stat) const;

	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

protected:
	TSharedPtr<F<PROJECT_NAME>PerformanceStatCache> Tracker;
};
