// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "Blueprint/IUserListEntry.h"
#include "Widgets/GameSettingListEntry.h"

#include "<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput.generated.h"

class U<PROJECT_NAME>SettingKeyboardInput;
class UCommonButtonBase;
class U<PROJECT_NAME>ButtonBase;
class UGameSettingPressAnyKey;

//////////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput
//////////////////////////////////////////////////////////////////////////

UCLASS(Abstract, Blueprintable, meta = (Category = "Settings", DisableNativeTick))
class U<PROJECT_NAME>SettingsListEntrySetting_KeyboardInput : public UGameSettingListEntry_Setting
{
	GENERATED_BODY()

public:
	virtual void SetSetting(UGameSetting* InSetting) override;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnEntryReleased() override;
	virtual void OnSettingChanged() override;

	void HandlePrimaryKeyClicked();
	void HandleSecondaryKeyClicked();
	void HandleClearClicked();

	void HandlePrimaryKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel);
	void HandleSecondaryKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel);
	void ChangeBinding(int32 BindSlot, FKey InKey);
	void HandleKeySelectionCanceled(UGameSettingPressAnyKey* PressAnyKeyPanel);

	void Refresh();

protected:
	UPROPERTY()
	U<PROJECT_NAME>SettingKeyboardInput* KeyboardInputSetting;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameSettingPressAnyKey> PressAnyKeyPanelClass;

private:	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	U<PROJECT_NAME>ButtonBase* Button_PrimaryKey;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	U<PROJECT_NAME>ButtonBase* Button_SecondaryKey;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	U<PROJECT_NAME>ButtonBase* Button_Clear;
};
