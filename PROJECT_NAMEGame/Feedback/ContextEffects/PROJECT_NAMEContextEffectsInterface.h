// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "<PROJECT_NAME>ContextEffectsInterface.generated.h"

class UStaticMeshComponent;
class UAnimSequenceBase;
class U<PROJECT_NAME>ContextEffectsLibrary;

/**
 *
 */
UENUM()
enum EEffectsContextMatchType
{
	ExactMatch,
	BestMatch
};

/**
 *
 */
 UINTERFACE(Blueprintable)
 class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ContextEffectsInterface : public UInterface
 {
	 GENERATED_BODY()

 };
 
 class <PROJECT_NAME_UPPER>GAME_API I<PROJECT_NAME>ContextEffectsInterface : public IInterface
 {
	 GENERATED_BODY()

 public:

	/** */
 	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AnimMotionEffect(const FName Bone
		, const FGameplayTag MotionEffect
		, USceneComponent* StaticMeshComponent
		, const FVector LocationOffset
		, const FRotator RotationOffset
		, const UAnimSequenceBase* AnimationSequence
		, const bool bHitSuccess
		, const FHitResult HitResult
		, FGameplayTagContainer Contexts
		, FVector VFXScale = FVector(1)
		, float AudioVolume = 1
		, float AudioPitch = 1);
 };

