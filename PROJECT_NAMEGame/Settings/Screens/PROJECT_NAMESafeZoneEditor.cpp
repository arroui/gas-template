// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>SafeZoneEditor.h"
#include "CommonButtonBase.h"
#include "Widgets/Layout/SSafeZone.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "Components/WidgetSwitcher.h"
#include "CommonRichTextBlock.h"
#include "GameSetting.h"
#include "GameSettingValueScalar.h"
#include "CommonUIUtils.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>"

namespace SafeZoneEditor
{
	const float JoystickDeadZone = 0.2f;
	const float SafeZoneChangeSpeed = 0.1f;	
}

U<PROJECT_NAME>SafeZoneEditor::U<PROJECT_NAME>SafeZoneEditor(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	Visibility = ESlateVisibility::Visible;
	bIsFocusable = true;
}

void U<PROJECT_NAME>SafeZoneEditor::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Switcher_SafeZoneMessage->SetActiveWidget(RichText_Default);
}

void U<PROJECT_NAME>SafeZoneEditor::NativeOnActivated()
{
	Super::NativeOnActivated();

	SSafeZone::SetGlobalSafeZoneScale(U<PROJECT_NAME>SettingsLocal::Get()->GetSafeZone());
	
	Button_Done->OnClicked().AddUObject(this, &U<PROJECT_NAME>SafeZoneEditor::HandleDoneClicked);

	Button_Back->SetVisibility((bCanCancel)? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bCanCancel)
	{
		Button_Back->OnClicked().AddUObject(this, &U<PROJECT_NAME>SafeZoneEditor::HandleBackClicked);
	}
}

bool U<PROJECT_NAME>SafeZoneEditor::ExecuteActionForSetting_Implementation(FGameplayTag ActionTag, UGameSetting* InSetting)
{
	TArray<UGameSetting*> ChildSettings = InSetting ? InSetting->GetChildSettings() : TArray<UGameSetting*>();
	if (ChildSettings.Num() > 0 && ChildSettings[0])
	{
		ValueSetting = Cast<UGameSettingValueScalar>(ChildSettings[0]);
	}

	return true;
}

FReply U<PROJECT_NAME>SafeZoneEditor::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	if (InAnalogEvent.GetKey() == EKeys::Gamepad_LeftY && FMath::Abs(InAnalogEvent.GetAnalogValue()) >= SafeZoneEditor::JoystickDeadZone)
	{
		const float SafeZoneMultiplier = FMath::Clamp(SSafeZone::GetGlobalSafeZoneScale().Get(1.0f) + InAnalogEvent.GetAnalogValue() * SafeZoneEditor::SafeZoneChangeSpeed, 0.0f, 1.0f);
		SSafeZone::SetGlobalSafeZoneScale(SafeZoneMultiplier >= 0 ? SafeZoneMultiplier : 0);
		
		return FReply::Handled();
	}
	return Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
}

FReply U<PROJECT_NAME>SafeZoneEditor::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const float SafeZoneMultiplier = FMath::Clamp(SSafeZone::GetGlobalSafeZoneScale().Get(1.0f) + InMouseEvent.GetWheelDelta() * SafeZoneEditor::SafeZoneChangeSpeed, 0.0f, 1.0f);
	SSafeZone::SetGlobalSafeZoneScale(SafeZoneMultiplier >= 0 ? SafeZoneMultiplier : 0);

	return FReply::Handled();
}

void U<PROJECT_NAME>SafeZoneEditor::HandleInputModeChanged(ECommonInputType InInputType)
{
	const FText KeyName = InInputType == ECommonInputType::Gamepad ? LOCTEXT("SafeZone_KeyToPress_Gamepad", "Left Stick") : LOCTEXT("SafeZone_KeyToPress_Mouse", "Mouse Wheel");
	RichText_Default->SetText(FText::Format(LOCTEXT("SafeZoneEditorInstructions", "Use <text color=\"FFFFFFFF\" fontface=\"black\">{0}</> to adjust the corners so it lines up with the edges of your display."), KeyName));
}

void U<PROJECT_NAME>SafeZoneEditor::HandleBackClicked()
{
	DeactivateWidget();
	SSafeZone::SetGlobalSafeZoneScale(U<PROJECT_NAME>SettingsLocal::Get()->GetSafeZone());
}

void U<PROJECT_NAME>SafeZoneEditor::HandleDoneClicked()
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