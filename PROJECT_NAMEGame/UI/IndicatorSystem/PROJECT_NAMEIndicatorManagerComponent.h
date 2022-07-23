// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "Delegates/DelegateCombinations.h"

#include "<PROJECT_NAME>IndicatorManagerComponent.generated.h"

class UIndicatorDescriptor;

/**
 * @class U<PROJECT_NAME>IndicatorManagerComponent
 */
UCLASS(BlueprintType, Blueprintable)
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>IndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>IndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static U<PROJECT_NAME>IndicatorManagerComponent* GetComponent(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
	
	UFUNCTION(BlueprintCallable, Category = Indicator)
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(U<PROJECT_NAME>IndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY()
	TArray<UIndicatorDescriptor*> Indicators;
};
