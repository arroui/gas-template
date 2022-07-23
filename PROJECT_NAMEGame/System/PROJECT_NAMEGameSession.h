// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "<PROJECT_NAME>GameSession.generated.h"


UCLASS(Config = Game)
class A<PROJECT_NAME>GameSession : public AGameSession
{
	GENERATED_BODY()

public:

	A<PROJECT_NAME>GameSession(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
};
