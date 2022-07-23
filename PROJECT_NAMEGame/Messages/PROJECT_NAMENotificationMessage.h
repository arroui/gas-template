// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "<PROJECT_NAME>NotificationMessage.generated.h"

<PROJECT_NAME_UPPER>GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_<PROJECT_NAME>_AddNotification_Message);

class APlayerController;
class APlayerState;

// A message destined for a transient log (e.g., an elimination feed or inventory pickup stream)
USTRUCT(BlueprintType)
struct <PROJECT_NAME_UPPER>GAME_API F<PROJECT_NAME>NotificationMessage
{
	GENERATED_BODY()

	// The destination channel
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	FGameplayTag TargetChannel;

	// The target player (if none is set then it will display for all local players)
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	APlayerState* TargetPlayer = nullptr;

	// The message to display
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	FText PayloadMessage;

	// Extra payload specific to the target channel (e.g., a style or definition asset)
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	FGameplayTag PayloadTag;

	// Extra payload specific to the target channel (e.g., a style or definition asset)
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	UObject* PayloadObject = nullptr;
};
