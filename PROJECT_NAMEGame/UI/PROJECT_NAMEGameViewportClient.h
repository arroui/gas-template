// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"

#include "<PROJECT_NAME>GameViewportClient.generated.h"

UCLASS(BlueprintType)
class U<PROJECT_NAME>GameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>GameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
};