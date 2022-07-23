// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>SettingAction_SafeZoneEditor.h"
#include "Settings/<PROJECT_NAME>GameSettingRegistry.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"
#include "Widgets/Layout/SSafeZone.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>Settings"

U<PROJECT_NAME>SettingAction_SafeZoneEditor::U<PROJECT_NAME>SettingAction_SafeZoneEditor()
{
	SafeZoneValueSetting = NewObject<U<PROJECT_NAME>SettingValueScalarDynamic_SafeZoneValue>();
	SafeZoneValueSetting->SetDevName(TEXT("SafeZoneValue"));
	SafeZoneValueSetting->SetDisplayName(LOCTEXT("SafeZoneValue_Name", "Safe Zone Value"));
	SafeZoneValueSetting->SetDescriptionRichText(LOCTEXT("SafeZoneValue_Description", "The safezone area percentage."));
	SafeZoneValueSetting->SetDefaultValue(0.0f);
	SafeZoneValueSetting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSafeZone));
	SafeZoneValueSetting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSafeZone));
	SafeZoneValueSetting->SetDisplayFormat([](double SourceValue, double NormalizedValue){ return FText::AsNumber(SourceValue); });
	SafeZoneValueSetting->SetSettingParent(this);
}

TArray<UGameSetting*> U<PROJECT_NAME>SettingAction_SafeZoneEditor::GetChildSettings()
{
	return { SafeZoneValueSetting };
}

void U<PROJECT_NAME>SettingValueScalarDynamic_SafeZoneValue::ResetToDefault()
{
	Super::ResetToDefault();
	SSafeZone::SetGlobalSafeZoneScale(TOptional<float>(DefaultValue.Get(0.0f)));
}

void U<PROJECT_NAME>SettingValueScalarDynamic_SafeZoneValue::RestoreToInitial()
{
	Super::RestoreToInitial();
	SSafeZone::SetGlobalSafeZoneScale(TOptional<float>(InitialValue));
}

#undef LOCTEXT_NAMESPACE