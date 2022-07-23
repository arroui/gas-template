// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>BrightnessEditor.h"
#include "CommonButtonBase.h"
#include "Widgets/Layout/SSafeZone.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "Components/WidgetSwitcher.h"
#include "CommonRichTextBlock.h"
#include "GameSetting.h"
#include "GameSettingValueScalar.h"
#include "CommonUIUtils.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>"

namespace BrightnessEditor
{
	const float JoystickDeadZone = 0.2f;
	const float SafeZoneChangeSpeed = 0.1f;
}

U<PROJECT_NAME>BrightnessEditor::U<PROJECT_NAME>BrightnessEditor(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	Visibility = ESlateVisibility::Visible;
	bIsFocusable = true;
}

void U<PROJECT_NAME>BrightnessEditor::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Switcher_SafeZoneMessage->SetActiveWidget(RichText_Default);
}

void U<PROJECT_NAME>BrightnessEditor::NativeOnActivated()
{
	Super::NativeOnActivated();

	SSafeZone::SetGlobalSafeZoneScale(U<PROJECT_NAME>SettingsLocal::Get()->GetSafeZone());
	
	Button_Done->OnClicked().AddUObject(this, &ThisClass::HandleDoneClicked);

	Button_Back->SetVisibility((bCanCancel)? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bCanCancel)
	{
		Button_Back->OnClicked().AddUObject(this, &ThisClass::HandleBackClicked);
	}
}

bool U<PROJECT_NAME>BrightnessEditor::ExecuteActionForSetting_Implementation(FGameplayTag ActionTag, UGameSetting* InSetting)
{
	TArray<UGameSetting*> ChildSettings = InSetting ? InSetting->GetChildSettings() : TArray<UGameSetting*>();
	if (ChildSettings.Num() > 0 && ChildSettings[0])
	{
		ValueSetting = Cast<UGameSettingValueScalar>(ChildSettings[0]);
	}

	return true;
}

FReply U<PROJECT_NAME>BrightnessEditor::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	if (InAnalogEvent.GetKey() == EKeys::Gamepad_LeftY && FMath::Abs(InAnalogEvent.GetAnalogValue()) >= BrightnessEditor::JoystickDeadZone)
	{
		const float SafeZoneMultiplier = FMath::Clamp(SSafeZone::GetGlobalSafeZoneScale().Get(1.0f) + InAnalogEvent.GetAnalogValue() * BrightnessEditor::SafeZoneChangeSpeed, 0.0f, 1.0f);
		SSafeZone::SetGlobalSafeZoneScale(SafeZoneMultiplier >= 0 ? SafeZoneMultiplier : 0);
		
		return FReply::Handled();
	}
	return Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
}

FReply U<PROJECT_NAME>BrightnessEditor::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const float SafeZoneMultiplier = FMath::Clamp(SSafeZone::GetGlobalSafeZoneScale().Get(1.0f) + InMouseEvent.GetWheelDelta() * BrightnessEditor::SafeZoneChangeSpeed, 0.0f, 1.0f);
	SSafeZone::SetGlobalSafeZoneScale(SafeZoneMultiplier >= 0 ? SafeZoneMultiplier : 0);

	return FReply::Handled();
}

void U<PROJECT_NAME>BrightnessEditor::HandleInputModeChanged(ECommonInputType InInputType)
{
	const FText KeyName = InInputType == ECommonInputType::Gamepad ? LOCTEXT("SafeZone_KeyToPress_Gamepad", "Left Stick") : LOCTEXT("SafeZone_KeyToPress_Mouse", "Mouse Wheel");
	RichText_Default->SetText(FText::Format(LOCTEXT("BrightnessAdjustInstructions", "Use <text color=\"FFFFFFFF\" fontface=\"black\">{0}</> to adjust the brightness"), KeyName));
}

void U<PROJECT_NAME>BrightnessEditor::HandleBackClicked()
{
	DeactivateWidget();
	SSafeZone::SetGlobalSafeZoneScale(U<PROJECT_NAME>SettingsLocal::Get()->GetSafeZone());
}

void U<PROJECT_NAME>BrightnessEditor::HandleDoneClicked()
{
	if (ValueSetting.IsValid())
	{
		ValueSetting.Get()->SetValue(SSafeZone::GetGlobalSafeZoneScale().Get(1.0f));
	}
	else
	{
		U<PROJECT_NAME>SettingsLocal::Get()->SetSafeZone(SSafeZone::GetGlobalSafeZoneScale().Get(1.0f));
	}
	OnSafeZoneSet.Broadcast();
	DeactivateWidget();
}

#undef LOCTEXT_NAMESPACE