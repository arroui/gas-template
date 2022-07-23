// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PerfStatWidgetBase.h"
#include "Performance/<PROJECT_NAME>PerformanceStatSubsystem.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>PerfStatWidgetBase

U<PROJECT_NAME>PerfStatWidgetBase::U<PROJECT_NAME>PerfStatWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

double U<PROJECT_NAME>PerfStatWidgetBase::FetchStatValue()
{
	if (CachedStatSubsystem == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<U<PROJECT_NAME>PerformanceStatSubsystem>();
			}
		}
	}

	if (CachedStatSubsystem)
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	else
	{
		return 0.0;
	}
}
