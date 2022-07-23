// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "<PROJECT_NAME>ContextEffectsSubsystem.generated.h"

class U<PROJECT_NAME>ContextEffectsLibrary;
class UNiagaraComponent;

/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "<PROJECT_NAME>ContextEffects"))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ContextEffectsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//
	UPROPERTY(config, EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, FGameplayTag> SurfaceTypeToContextMap;
};

/**
 *
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ContextEffectsSet : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TSet<U<PROJECT_NAME>ContextEffectsLibrary*> <PROJECT_NAME>ContextEffectsLibraries;
};


/**
 * 
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ContextEffectsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void SpawnContextEffects(
		const AActor* SpawningActor
		, USceneComponent* AttachToComponent
		, const FName AttachPoint
		, const FVector LocationOffset
		, const FRotator RotationOffset
		, FGameplayTag Effect
		, FGameplayTagContainer Contexts
		, TArray<UAudioComponent*>& AudioOut
		, TArray<UNiagaraComponent*>& NiagaraOut
		, FVector VFXScale = FVector(1)
		, float AudioVolume = 1
		, float AudioPitch = 1);

	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	bool GetContextFromSurfaceType(TEnumAsByte<EPhysicalSurface> PhysicalSurface, FGameplayTag& Context);

	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void LoadAndAddContextEffectsLibraries(AActor* OwningActor, TSet<TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>> ContextEffectsLibraries);

	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void UnloadAndRemoveContextEffectsLibraries(AActor* OwningActor);

private:

	UPROPERTY(Transient)
	TMap<AActor*, U<PROJECT_NAME>ContextEffectsSet*> ActiveActorEffectsMap;

};