// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ControllerDisconnectedScreen.h"

#include "Components/HorizontalBox.h"
#include "HAL/PlatformMisc.h"

void U<PROJECT_NAME>ControllerDisconnectedScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	HBox_SwitchUser->SetVisibility(ESlateVisibility::Collapsed);

//#if PLATFORM_HAS_STRICT_CONTROLLER_PAIRING
//	if (FPlatformMisc::GetControllerIdForUnpairedControllers() != INDEX_NONE)
//	{
//		HBox_SwitchUser->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
//	}
//#endif
}

