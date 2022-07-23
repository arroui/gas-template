// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameSettingAction.h"
#include "GameSettingValueScalarDynamic.h"

#include "<PROJECT_NAME>SettingAction_SafeZoneEditor.generated.h"


UCLASS()
class U<PROJECT_NAME>SettingValueScalarDynamic_SafeZoneValue : public UGameSettingValueScalarDynamic
{
	GENERATED_BODY()

public:
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;
};

UCLASS()
class U<PROJECT_NAME>SettingAction_SafeZoneEditor : public UGameSettingAction
{
	GENERATED_BODY()
	
public:
	U<PROJECT_NAME>SettingAction_SafeZoneEditor();
	virtual TArray<UGameSetting*> GetChildSettings() override;

private:
	UPROPERTY()
	U<PROJECT_NAME>SettingValueScalarDynamic_SafeZoneValue* SafeZoneValueSetting;
};