// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameSettingRegistry.h"
#include "DataSource/GameSettingDataSourceDynamic.h"

#include "<PROJECT_NAME>GameSettingRegistry.generated.h"

//--------------------------------------
// U<PROJECT_NAME>GameSettingRegistry
//--------------------------------------

class U<PROJECT_NAME>LocalPlayer;
class U<PROJECT_NAME>ClientSettingsRecord;
class U<PROJECT_NAME>GameUserSettings;
class UGameSettingCollection;
class UGameSetting;
struct FGameSettingsFilterState;
class U<PROJECT_NAME>ContentControlSettingRegistry;

DECLARE_LOG_CATEGORY_EXTERN(Log<PROJECT_NAME>GameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(U<PROJECT_NAME>LocalPlayer, GetSharedSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(U<PROJECT_NAME>SettingsShared, FunctionOrPropertyName)		\
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(U<PROJECT_NAME>LocalPlayer, GetLocalSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(U<PROJECT_NAME>SettingsLocal, FunctionOrPropertyName)		\
	}))

/**
 * 
 */
UCLASS()
class U<PROJECT_NAME>GameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>GameSettingRegistry();

	static U<PROJECT_NAME>GameSettingRegistry* Get(U<PROJECT_NAME>LocalPlayer* InLocalPlayer);
	
	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	UGameSettingCollection* InitializeVideoSettings(U<PROJECT_NAME>LocalPlayer* InLocalPlayer);
	void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, U<PROJECT_NAME>LocalPlayer* InLocalPlayer);
	void AddPerformanceStatPage(UGameSettingCollection* Screen, U<PROJECT_NAME>LocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeAudioSettings(U<PROJECT_NAME>LocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGameplaySettings(U<PROJECT_NAME>LocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeMouseAndKeyboardSettings(U<PROJECT_NAME>LocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGamepadSettings(U<PROJECT_NAME>LocalPlayer* InLocalPlayer);

	UPROPERTY()
	UGameSettingCollection* VideoSettings;

	UPROPERTY()
	UGameSettingCollection* AudioSettings;

	UPROPERTY()
	UGameSettingCollection* GameplaySettings;

	UPROPERTY()
	UGameSettingCollection* MouseAndKeyboardSettings;

	UPROPERTY()
	UGameSettingCollection* GamepadSettings;
};
