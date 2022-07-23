// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "<PROJECT_NAME>VerbMessage.h"
#include "GameplayEffectTypes.h"

#include "<PROJECT_NAME>VerbMessageHelpers.generated.h"

class APlayerState;
class APlayerController;
struct FGameplayCueParameters;


UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>VerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>")
	static FGameplayCueParameters VerbMessageToCueParameters(const F<PROJECT_NAME>VerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>")
	static F<PROJECT_NAME>VerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
