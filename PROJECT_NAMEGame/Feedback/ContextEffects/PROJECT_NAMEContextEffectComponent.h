// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "<PROJECT_NAME>ContextEffectsInterface.h"
#include "UObject/SoftObjectPtr.h"
#include "<PROJECT_NAME>ContextEffectComponent.generated.h"

class U<PROJECT_NAME>ContextEffectsLibrary;
class UNiagaraComponent;
class UAudioComponent;

UCLASS( ClassGroup=(Custom), hidecategories = (Variable, Tags, ComponentTick, ComponentReplication, Activation, Cooking, AssetUserData, Collision), CollapseCategories, meta=(BlueprintSpawnableComponent) )
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ContextEffectComponent : public UActorComponent, public I<PROJECT_NAME>ContextEffectsInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	U<PROJECT_NAME>ContextEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// AnimMotionEffect Implementation
	UFUNCTION(BlueprintCallable)
	virtual void AnimMotionEffect_Implementation(const FName Bone, const FGameplayTag MotionEffect, USceneComponent* StaticMeshComponent,
		const FVector LocationOffset, const FRotator RotationOffset, const UAnimSequenceBase* AnimationSequence,
		const bool bHitSuccess, const FHitResult HitResult, FGameplayTagContainer Contexts,
		FVector VFXScale = FVector(1), float AudioVolume = 1, float AudioPitch = 1) override;

	// Auto-Convert Physical Surface from Trace Result to Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bConvertPhysicalSurfaceToContext = true;

	// Default Contexts
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DefaultEffectContexts;

	// Default Libraries for this Actor
	UPROPERTY(EditAnywhere)
	TSet<TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>> DefaultContextEffectsLibraries;

	UFUNCTION(BlueprintCallable)
	void UpdateEffectContexts(FGameplayTagContainer NewEffectContexts);

	UFUNCTION(BlueprintCallable)
	void UpdateLibraries(TSet<TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>> NewContextEffectsLibraries);

private:
	UPROPERTY(Transient)
	FGameplayTagContainer CurrentContexts;

	UPROPERTY(Transient)
	TSet<TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>> CurrentContextEffectsLibraries;

	UPROPERTY(Transient)
	TArray<UAudioComponent*> ActiveAudioComponents;

	UPROPERTY(Transient)
	TArray<UNiagaraComponent*> ActiveNiagaraComponents;
};
