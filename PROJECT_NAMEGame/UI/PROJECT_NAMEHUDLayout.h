// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "<PROJECT_NAME>ActivatableWidget.h"

#include "<PROJECT_NAME>HUDLayout.generated.h"


/**
 * U<PROJECT_NAME>HUDLayout
 *
 *	Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "<PROJECT_NAME> HUD Layout", Category = "<PROJECT_NAME>|HUD"))
class U<PROJECT_NAME>HUDLayout : public U<PROJECT_NAME>ActivatableWidget
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>HUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
