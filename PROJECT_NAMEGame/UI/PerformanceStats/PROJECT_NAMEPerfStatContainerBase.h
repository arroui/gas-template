// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Performance/<PROJECT_NAME>PerformanceStatTypes.h"

#include "<PROJECT_NAME>PerfStatContainerBase.generated.h"

/**
 * U<PROJECT_NAME>PerfStatsContainerBase
 *
 * Panel that contains a set of U<PROJECT_NAME>PerfStatWidgetBase widgets and manages
 * their visibility based on user settings.
 */
 UCLASS(Abstract)
class U<PROJECT_NAME>PerfStatContainerBase : public UCommonUserWidget
{
public:
	U<PROJECT_NAME>PerfStatContainerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

	//~UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

	UFUNCTION(BlueprintCallable)
	void UpdateVisibilityOfChildren();

protected:
	// Are we showing text or graph stats?
	UPROPERTY(EditAnywhere, Category=Display)
	E<PROJECT_NAME>StatDisplayMode StatDisplayModeFilter = E<PROJECT_NAME>StatDisplayMode::TextAndGraph;
};
