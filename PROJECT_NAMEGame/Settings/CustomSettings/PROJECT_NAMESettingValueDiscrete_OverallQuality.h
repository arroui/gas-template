// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameSettingValueDiscrete.h"

#include "<PROJECT_NAME>SettingValueDiscrete_OverallQuality.generated.h"

struct FScreenOverallQualityRHI;

UCLASS()
class U<PROJECT_NAME>SettingValueDiscrete_OverallQuality : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	U<PROJECT_NAME>SettingValueDiscrete_OverallQuality();

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
	
	int32 GetCustomOptionIndex() const;
	int32 GetOverallQualityLevel() const;

	TArray<FText> Options;
	TArray<FText> OptionsWithCustom;

};