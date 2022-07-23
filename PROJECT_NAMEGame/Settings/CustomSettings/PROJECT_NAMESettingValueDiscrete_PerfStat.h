// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameSettingValueDiscrete.h"
#include "Performance/<PROJECT_NAME>PerformanceStatTypes.h"

#include "<PROJECT_NAME>SettingValueDiscrete_PerfStat.generated.h"

struct FScreenOverallQualityRHI;

UCLASS()
class U<PROJECT_NAME>SettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	U<PROJECT_NAME>SettingValueDiscrete_PerfStat();

	void SetStat(E<PROJECT_NAME>DisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, E<PROJECT_NAME>StatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<E<PROJECT_NAME>StatDisplayMode> DisplayModes;

	E<PROJECT_NAME>DisplayablePerformanceStat StatToDisplay;
	E<PROJECT_NAME>StatDisplayMode InitialMode;
};