// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PerformanceSettings.h"
#include "Engine/PlatformSettings.h"

//////////////////////////////////////////////////////////////////////

U<PROJECT_NAME>PlatformSpecificRenderingSettings::U<PROJECT_NAME>PlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const U<PROJECT_NAME>PlatformSpecificRenderingSettings* U<PROJECT_NAME>PlatformSpecificRenderingSettings::Get()
{
	U<PROJECT_NAME>PlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

U<PROJECT_NAME>PerformanceSettings::U<PROJECT_NAME>PerformanceSettings()
{
	PerPlatformSettings.Initialize(U<PROJECT_NAME>PlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	F<PROJECT_NAME>PerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (E<PROJECT_NAME>DisplayablePerformanceStat PerfStat : TEnumRange<E<PROJECT_NAME>DisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}
