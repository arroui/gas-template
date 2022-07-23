// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameInstance.h"
#include "Player/<PROJECT_NAME>PlayerController.h"

U<PROJECT_NAME>GameInstance::U<PROJECT_NAME>GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>GameInstance::Init()
{
	Super::Init();
}

void U<PROJECT_NAME>GameInstance::Shutdown()
{
	Super::Shutdown();
}

A<PROJECT_NAME>PlayerController* U<PROJECT_NAME>GameInstance::GetPrimaryPlayerController() const
{
	return Cast<A<PROJECT_NAME>PlayerController>(Super::GetPrimaryPlayerController(false));
}