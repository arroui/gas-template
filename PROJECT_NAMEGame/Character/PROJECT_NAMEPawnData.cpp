// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PawnData.h"
#include "GameFramework/Pawn.h"
#include "Character/<PROJECT_NAME>PawnExtensionComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySet.h"

U<PROJECT_NAME>PawnData::U<PROJECT_NAME>PawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}
