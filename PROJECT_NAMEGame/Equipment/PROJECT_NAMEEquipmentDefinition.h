// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "<PROJECT_NAME>EquipmentDefinition.generated.h"

class U<PROJECT_NAME>AbilitySet;
class U<PROJECT_NAME>EquipmentInstance;

USTRUCT()
struct F<PROJECT_NAME>EquipmentActorToSpawn
{
	GENERATED_BODY()

	F<PROJECT_NAME>EquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


/**
 * U<PROJECT_NAME>EquipmentDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class U<PROJECT_NAME>EquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>EquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<U<PROJECT_NAME>EquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const U<PROJECT_NAME>AbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<F<PROJECT_NAME>EquipmentActorToSpawn> ActorsToSpawn;
};
