// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameViewportClient.h"

#include "CommonUISettings.h"
#include "NativeGameplayTags.h"
#include "ICommonUIModule.h"

namespace GameViewportTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_HardwareCursor, "Platform.Trait.Input.HardwareCursor");
}

U<PROJECT_NAME>GameViewportClient::U<PROJECT_NAME>GameViewportClient()
	: Super(FObjectInitializer::Get())
{
}

void U<PROJECT_NAME>GameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	
	// We have software cursors set up in our project settings for console/mobile use, but on desktop we're fine with
	// the standard hardware cursors
	const bool UseHardwareCursor = ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GameViewportTags::TAG_Platform_Trait_Input_HardwareCursor);
	SetUseSoftwareCursorWidgets(!UseHardwareCursor);
}