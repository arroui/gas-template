// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"

#include "<PROJECT_NAME>ControllerDisconnectedScreen.generated.h"

class UHorizontalBox;

UCLASS(Abstract, BlueprintType, Blueprintable)
class U<PROJECT_NAME>ControllerDisconnectedScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	void NativeOnActivated() override;

private:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HBox_SwitchUser;
};