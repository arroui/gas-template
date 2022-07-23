// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>HUDLayout.h"
#include "Input/UIActionBindingHandle.h"
#include "Engine/EngineBaseTypes.h"
#include "Input/CommonInputMode.h"
#include "NativeGameplayTags.h"
#include "CommonUIExtensions.h"
#include "Input/CommonUIInputTypes.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");

U<PROJECT_NAME>HUDLayout::U<PROJECT_NAME>HUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>HUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
}

void U<PROJECT_NAME>HUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, EscapeMenuClass);
	}
}