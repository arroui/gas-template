// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "<PROJECT_NAME>AbilitySimpleFailureMessage.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE);

USTRUCT(BlueprintType)
struct F<PROJECT_NAME>AbilitySimpleFailureMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	APlayerController* PlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	FText UserFacingReason;
};
