// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>SettingValueDiscrete_PerfStat.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "GameSettingFilterState.h"
#include "Performance/<PROJECT_NAME>PerformanceSettings.h"
#include "CommonUIVisibilitySubsystem.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>Settings"

//////////////////////////////////////////////////////////////////////

class FGameSettingEditCondition_PerfStatAllowed : public FGameSettingEditCondition
{
public:
	static TSharedRef<FGameSettingEditCondition_PerfStatAllowed> Get(E<PROJECT_NAME>DisplayablePerformanceStat Stat)
	{
		return MakeShared<FGameSettingEditCondition_PerfStatAllowed>(Stat);
	}

	FGameSettingEditCondition_PerfStatAllowed(E<PROJECT_NAME>DisplayablePerformanceStat Stat)
		: AssociatedStat(Stat)
	{
	}

	//~FGameSettingEditCondition interface
	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		const FGameplayTagContainer& VisibilityTags = UCommonUIVisibilitySubsystem::GetChecked(InLocalPlayer)->GetVisibilityTags();

		bool bCanShowStat = false;
		for (const F<PROJECT_NAME>PerformanceStatGroup& Group : GetDefault<U<PROJECT_NAME>PerformanceSettings>()->UserFacingPerformanceStats) //@TODO: Move this stuff to per-platform instead of doing vis queries too?
		{
			if (Group.AllowedStats.Contains(AssociatedStat))
			{
				const bool bShowGroup = (Group.VisibilityQuery.IsEmpty() || Group.VisibilityQuery.Matches(VisibilityTags));
				if (bShowGroup)
				{
					bCanShowStat = true;
					break;
				}
			}
		}

		if (!bCanShowStat)
		{
			InOutEditState.Hide(TEXT("Stat is not listed in U<PROJECT_NAME>PerformanceSettings or is suppressed by current platform traits"));
		}
	}
	//~End of FGameSettingEditCondition interface

private:
	E<PROJECT_NAME>DisplayablePerformanceStat AssociatedStat;
};

//////////////////////////////////////////////////////////////////////

U<PROJECT_NAME>SettingValueDiscrete_PerfStat::U<PROJECT_NAME>SettingValueDiscrete_PerfStat()
{
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::SetStat(E<PROJECT_NAME>DisplayablePerformanceStat InStat)
{
	StatToDisplay = InStat;
	SetDevName(FName(*FString::Printf(TEXT("PerfStat_%d"), (int32)StatToDisplay)));
	AddEditCondition(FGameSettingEditCondition_PerfStatAllowed::Get(StatToDisplay));
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::AddMode(FText&& Label, E<PROJECT_NAME>StatDisplayMode Mode)
{
	Options.Emplace(MoveTemp(Label));
	DisplayModes.Add(Mode);
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::OnInitialized()
{
	Super::OnInitialized();

	AddMode(LOCTEXT("PerfStatDisplayMode_None", "None"), E<PROJECT_NAME>StatDisplayMode::Hidden);
	AddMode(LOCTEXT("PerfStatDisplayMode_TextOnly", "Text Only"), E<PROJECT_NAME>StatDisplayMode::TextOnly);
	AddMode(LOCTEXT("PerfStatDisplayMode_GraphOnly", "Graph Only"), E<PROJECT_NAME>StatDisplayMode::GraphOnly);
	AddMode(LOCTEXT("PerfStatDisplayMode_TextAndGraph", "Text and Graph"), E<PROJECT_NAME>StatDisplayMode::TextAndGraph);
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::StoreInitial()
{
	const U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get();
	InitialMode = Settings->GetPerfStatDisplayState(StatToDisplay);
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::ResetToDefault()
{
	U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, E<PROJECT_NAME>StatDisplayMode::Hidden);
	NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::RestoreToInitial()
{
	U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, InitialMode);
	NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
}

void U<PROJECT_NAME>SettingValueDiscrete_PerfStat::SetDiscreteOptionByIndex(int32 Index)
{
	if (DisplayModes.IsValidIndex(Index))
	{
		const E<PROJECT_NAME>StatDisplayMode DisplayMode = DisplayModes[Index];

		U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get();
		Settings->SetPerfStatDisplayState(StatToDisplay, DisplayMode);
	}
	NotifySettingChanged(EGameSettingChangeReason::Change);
}

int32 U<PROJECT_NAME>SettingValueDiscrete_PerfStat::GetDiscreteOptionIndex() const
{
	const U<PROJECT_NAME>SettingsLocal* Settings = U<PROJECT_NAME>SettingsLocal::Get();
	return DisplayModes.Find(Settings->GetPerfStatDisplayState(StatToDisplay));
}

TArray<FText> U<PROJECT_NAME>SettingValueDiscrete_PerfStat::GetDiscreteOptions() const
{
	return Options;
}

#undef LOCTEXT_NAMESPACE