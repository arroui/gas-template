// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/<PROJECT_NAME>SimulatedInputWidget.h"
#include "<PROJECT_NAME>TouchRegion.generated.h"

/**
 * A "Touch Region" is used to define an area on the screen that should trigger some
 * input when the user presses a finger on it
 */
UCLASS(meta=( DisplayName="<PROJECT_NAME> Touch Region" ))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>TouchRegion : public U<PROJECT_NAME>SimulatedInputWidget
{
	GENERATED_BODY()
	
public:
	
	//~ Begin UUserWidget
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface

	UFUNCTION(BlueprintCallable)
	bool ShouldSimulateInput() const { return bShouldSimulateInput; }

protected:

	/** True while this widget is being touched */
	bool bShouldSimulateInput = false;
};
