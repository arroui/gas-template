// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "<PROJECT_NAME>PerfStatWidgetBase.generated.h"

class U<PROJECT_NAME>PerformanceStatSubsystem;

/**
 * U<PROJECT_NAME>PerfStatWidgetBase
 *
 * Base class for a widget that displays a single stat, e.g., FPS, ping, etc...
 */
 UCLASS(Abstract)
class U<PROJECT_NAME>PerfStatWidgetBase : public UCommonUserWidget
{
public:
	U<PROJECT_NAME>PerfStatWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

public:
	// Returns the stat this widget is supposed to display
	UFUNCTION(BlueprintPure)
	E<PROJECT_NAME>DisplayablePerformanceStat GetStatToDisplay() const
	{
		return StatToDisplay;
	}

	// Polls for the value of this stat (unscaled)
	UFUNCTION(BlueprintPure)
	double FetchStatValue();

protected:
	// Cached subsystem pointer
	UPROPERTY(Transient)
	TObjectPtr<U<PROJECT_NAME>PerformanceStatSubsystem> CachedStatSubsystem;

	// The stat to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	E<PROJECT_NAME>DisplayablePerformanceStat StatToDisplay;
 };
