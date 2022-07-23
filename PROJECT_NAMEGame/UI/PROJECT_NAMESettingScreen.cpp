// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>SettingScreen.h"
#include "Settings/<PROJECT_NAME>GameSettingRegistry.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"
#include "Input/CommonUIInputTypes.h"

void U<PROJECT_NAME>SettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* U<PROJECT_NAME>SettingScreen::CreateRegistry()
{
	U<PROJECT_NAME>GameSettingRegistry* NewRegistry = NewObject<U<PROJECT_NAME>GameSettingRegistry>();

	if (U<PROJECT_NAME>LocalPlayer* LocalPlayer = CastChecked<U<PROJECT_NAME>LocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void U<PROJECT_NAME>SettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void U<PROJECT_NAME>SettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void U<PROJECT_NAME>SettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void U<PROJECT_NAME>SettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}