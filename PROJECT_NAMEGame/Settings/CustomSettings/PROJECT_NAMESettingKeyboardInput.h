// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameSettingValue.h"
#include "EnhancedActionKeyMapping.h"

#include "<PROJECT_NAME>SettingKeyboardInput.generated.h"

//--------------------------------------
// U<PROJECT_NAME>SettingKeyboardInput
//--------------------------------------

class UPlayerMappableInputConfig;

struct FKeyboardOption
{
	FKeyboardOption() = default;
	
	FEnhancedActionKeyMapping InputMapping {};
	
	const UPlayerMappableInputConfig* OwningConfig = nullptr;

	void ResetToDefault();

	/** Store the currently set FKey that this is bound to */
	void SetInitialValue(FKey InKey);

	/** Get the most recently stored initial value */
	FKey GetInitialStoredValue() const { return InitialMapping; };

private:

	/** The key that this option is bound to initially, used in case the user wants to cancel their mapping */
	FKey InitialMapping;
};

UCLASS()
class U<PROJECT_NAME>SettingKeyboardInput : public UGameSettingValue
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>SettingKeyboardInput();

	/** Initalize this setting widget based off the given mapping */
	void SetInputData(FEnhancedActionKeyMapping& BaseMapping, const UPlayerMappableInputConfig* InOwningConfig, int32 InKeyBindSlot);

	FText GetPrimaryKeyText() const;
	FText GetSecondaryKeyText() const;
	
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	bool ChangeBinding(int32 InKeyBindSlot, FKey NewKey);
	
	FText GetSettingDisplayName() const { return FirstMappableOption.InputMapping.PlayerMappableOptions.DisplayName; }
	
protected:
	/** U<PROJECT_NAME>Setting */
	virtual void OnInitialized() override;

protected:

	FKeyboardOption FirstMappableOption;
	FKeyboardOption SecondaryMappableOption;
};
