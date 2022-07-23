// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>SettingValueDiscrete_MobileFPSType.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "Performance/<PROJECT_NAME>PerformanceSettings.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>Settings"

U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType()
{
}

void U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const U<PROJECT_NAME>PlatformSpecificRenderingSettings* PlatformSettings = U<PROJECT_NAME>PlatformSpecificRenderingSettings::Get();
	const U<PROJECT_NAME>SettingsLocal* UserSettings = U<PROJECT_NAME>SettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (U<PROJECT_NAME>SettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return U<PROJECT_NAME>SettingsLocal::GetDefaultMobileFrameRate();
}

FText U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::GetValue() const
{
	return U<PROJECT_NAME>SettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void U<PROJECT_NAME>SettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	U<PROJECT_NAME>SettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE
