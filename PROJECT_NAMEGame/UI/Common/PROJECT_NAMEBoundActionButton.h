// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionButton.h"
#include "<PROJECT_NAME>BoundActionButton.generated.h"

/**
 * 
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>BoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	void HandleInputMethodChanged(ECommonInputType NewInputMethod);

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> TouchStyle;
};
