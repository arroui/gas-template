// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "<PROJECT_NAME>PawnComponent.generated.h"


UINTERFACE(BlueprintType)
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ReadyInterface : public UInterface
{
	GENERATED_BODY()
};

class I<PROJECT_NAME>ReadyInterface
{
	GENERATED_BODY()

public:
	virtual bool IsPawnComponentReadyToInitialize() const = 0;
};




/**
 * U<PROJECT_NAME>PawnComponent
 *
 *	An actor component that can be used for adding custom behavior to pawns.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>PawnComponent : public UPawnComponent, public I<PROJECT_NAME>ReadyInterface
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>PawnComponent(const FObjectInitializer& ObjectInitializer);

	virtual bool IsPawnComponentReadyToInitialize() const override { return true; }
};
