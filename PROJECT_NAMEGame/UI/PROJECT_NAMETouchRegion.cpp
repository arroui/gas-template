// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/<PROJECT_NAME>TouchRegion.h"
#include "CommonHardwareVisibilityBorder.h"

FReply U<PROJECT_NAME>TouchRegion::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bShouldSimulateInput = true;
	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

FReply U<PROJECT_NAME>TouchRegion::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// Input our associatied key as long as the player is touching within our bounds
	//InputKeyValue(FVector::OneVector);
	bShouldSimulateInput = true;
	// Continuously trigger the input if we should
	return Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
}

FReply U<PROJECT_NAME>TouchRegion::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bShouldSimulateInput = false;
	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

void U<PROJECT_NAME>TouchRegion::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(bShouldSimulateInput)
	{
		InputKeyValue(FVector::OneVector);
	}
}
