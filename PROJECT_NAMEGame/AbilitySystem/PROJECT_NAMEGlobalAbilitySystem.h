// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"

#include "<PROJECT_NAME>GlobalAbilitySystem.generated.h"

class U<PROJECT_NAME>AbilitySystemComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<U<PROJECT_NAME>AbilitySystemComponent*, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayAbility> Ability, U<PROJECT_NAME>AbilitySystemComponent* ASC);
	void RemoveFromASC(U<PROJECT_NAME>AbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<U<PROJECT_NAME>AbilitySystemComponent*, FActiveGameplayEffectHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayEffect> Effect, U<PROJECT_NAME>AbilitySystemComponent* ASC);
	void RemoveFromASC(U<PROJECT_NAME>AbilitySystemComponent* ASC);
	void RemoveFromAll();
};

UCLASS()
class U<PROJECT_NAME>GlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>GlobalAbilitySystem();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="<PROJECT_NAME>")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="<PROJECT_NAME>")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "<PROJECT_NAME>")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "<PROJECT_NAME>")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(U<PROJECT_NAME>AbilitySystemComponent* ASC);

	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(U<PROJECT_NAME>AbilitySystemComponent* ASC);

private:
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<U<PROJECT_NAME>AbilitySystemComponent*> RegisteredASCs;
};
