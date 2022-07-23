// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/SoftObjectPath.h"
#include "GameplayTagContainer.h"
#include "<PROJECT_NAME>ContextEffectsLibrary.generated.h"

class USoundBase;
class UNiagaraSystem;

/**
 *
 */
UENUM()
enum class EContextEffectsLibraryLoadState : uint8 {
	Unloaded = 0,
	Loading = 1,
	Loaded = 2
};

/**
 *
 */
USTRUCT(BlueprintType)
struct <PROJECT_NAME_UPPER>GAME_API F<PROJECT_NAME>ContextEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer Context;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowedClasses = "SoundBase, NiagaraSystem"))
	TArray<FSoftObjectPath> Effects;

};

/**
 *
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ActiveContextEffects : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FGameplayTag EffectTag;

	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer Context;

	UPROPERTY(VisibleAnywhere)
	TArray<USoundBase*> Sounds;

	UPROPERTY(VisibleAnywhere)
	TArray<UNiagaraSystem*> NiagaraSystems;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(F<PROJECT_NAME>ContextEffectLibraryLoadingComplete, TArray<U<PROJECT_NAME>ActiveContextEffects*>, <PROJECT_NAME>ActiveContextEffects);

/**
 * 
 */
UCLASS(BlueprintType)
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ContextEffectsLibrary : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<F<PROJECT_NAME>ContextEffects> ContextEffects;

	UFUNCTION(BlueprintCallable)
	void GetEffects(const FGameplayTag Effect, const FGameplayTagContainer Context, TArray<USoundBase*>& Sounds, TArray<UNiagaraSystem*>& NiagaraSystems);

	UFUNCTION(BlueprintCallable)
	void LoadEffects();

	EContextEffectsLibraryLoadState GetContextEffectsLibraryLoadState();

private:
	void LoadEffectsInternal();

	void <PROJECT_NAME>ContextEffectLibraryLoadingComplete(TArray<U<PROJECT_NAME>ActiveContextEffects*> <PROJECT_NAME>ActiveContextEffects);

	UPROPERTY(Transient)
	TArray< U<PROJECT_NAME>ActiveContextEffects*> ActiveContextEffects;

	UPROPERTY(Transient)
	EContextEffectsLibraryLoadState EffectsLoadState = EContextEffectsLibraryLoadState::Unloaded;
};
