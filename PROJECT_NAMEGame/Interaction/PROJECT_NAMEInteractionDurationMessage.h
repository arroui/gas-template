// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "<PROJECT_NAME>InteractionDurationMessage.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_INTERACTION_DURATION_MESSAGE);

USTRUCT(BlueprintType)
struct F<PROJECT_NAME>InteractionDurationMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	float Duration = 0;
};
