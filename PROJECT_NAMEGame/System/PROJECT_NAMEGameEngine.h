// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "<PROJECT_NAME>GameEngine.generated.h"


UCLASS()
class U<PROJECT_NAME>GameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>GameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};
