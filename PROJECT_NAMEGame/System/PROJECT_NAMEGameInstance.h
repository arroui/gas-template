// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CommonGameInstance.h"
#include "<PROJECT_NAME>GameInstance.generated.h"

class U<PROJECT_NAME>SocialManager;
class U<PROJECT_NAME>SocialParty;
class USocialManager;
class A<PROJECT_NAME>PlayerController;

UCLASS(Config = Game)
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>GameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>GameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	A<PROJECT_NAME>PlayerController* GetPrimaryPlayerController() const;

protected:

	virtual void Init() override;
	virtual void Shutdown() override;
};
