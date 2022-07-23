// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "UObject/ScriptInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IndicatorLibrary.generated.h"

class U<PROJECT_NAME>IndicatorManagerComponent;

UCLASS()
class <PROJECT_NAME_UPPER>GAME_API UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UIndicatorLibrary();
	
	/**  */
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static U<PROJECT_NAME>IndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};
