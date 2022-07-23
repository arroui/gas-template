// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/Widgets/<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput.h"
#include "CommonButtonBase.h"
#include "CommonUIExtensions.h"
#include "NativeGameplayTags.h"
#include "Widgets/Misc/GameSettingPressAnyKey.h"
#include "Settings/CustomSettings/<PROJECT_NAME>SettingKeyboardInput.h"
#include "UI/Foundation/<PROJECT_NAME>ButtonBase.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>Settings"

UE_DEFINE_GAMEPLAY_TAG_STATIC(PressAnyKeyLayer, "UI.Layer.Modal");

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::SetSetting(UGameSetting* InSetting)
{
	KeyboardInputSetting = CastChecked<U<PROJECT_NAME>SettingKeyboardInput>(InSetting);

	Super::SetSetting(InSetting);

	Refresh();
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_PrimaryKey->OnClicked().AddUObject(this, &ThisClass::HandlePrimaryKeyClicked);
	Button_SecondaryKey->OnClicked().AddUObject(this, &ThisClass::HandleSecondaryKeyClicked);
	Button_Clear->OnClicked().AddUObject(this, &ThisClass::HandleClearClicked);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::HandlePrimaryKeyClicked()
{
	UGameSettingPressAnyKey* PressAnyKeyPanel = CastChecked<UGameSettingPressAnyKey>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, PressAnyKeyPanelClass));
	PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandlePrimaryKeySelected, PressAnyKeyPanel);
	PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::HandleSecondaryKeyClicked()
{
	UGameSettingPressAnyKey* PressAnyKeyPanel = CastChecked<UGameSettingPressAnyKey>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, PressAnyKeyPanelClass));
	PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandleSecondaryKeySelected, PressAnyKeyPanel);
	PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::HandlePrimaryKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(0, InKey);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::HandleSecondaryKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(1, InKey);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::ChangeBinding(int32 InKeyBindSlot, FKey InKey)
{
	KeyboardInputSetting->ChangeBinding(InKeyBindSlot, InKey);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::HandleKeySelectionCanceled(UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelectionCanceled.RemoveAll(this);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::HandleClearClicked()
{
	KeyboardInputSetting->ChangeBinding(0, EKeys::Invalid);
	KeyboardInputSetting->ChangeBinding(1, EKeys::Invalid);
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::OnSettingChanged()
{
	Refresh();
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::Refresh()
{
	if (ensure(KeyboardInputSetting))
	{
		Button_PrimaryKey->SetButtonText(KeyboardInputSetting->GetPrimaryKeyText());
		Button_SecondaryKey->SetButtonText(KeyboardInputSetting->GetSecondaryKeyText());
	}
}

void U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput::NativeOnEntryReleased()
{
	Super::NativeOnEntryReleased();

	KeyboardInputSetting = nullptr;
}

#undef LOCTEXT_NAMESPACE