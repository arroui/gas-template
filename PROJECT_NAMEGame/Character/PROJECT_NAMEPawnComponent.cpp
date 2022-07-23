// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PawnComponent.h"


U<PROJECT_NAME>PawnComponent::U<PROJECT_NAME>PawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}
