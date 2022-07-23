// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "Character/<PROJECT_NAME>PawnComponent.h"
#include "<PROJECT_NAME>HealthComponent.generated.h"


class U<PROJECT_NAME>AbilitySystemComponent;
class U<PROJECT_NAME>HealthSet;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(F<PROJECT_NAME>Health_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(F<PROJECT_NAME>Health_AttributeChanged, U<PROJECT_NAME>HealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);


/**
 * E<PROJECT_NAME>DeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class E<PROJECT_NAME>DeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * U<PROJECT_NAME>HealthComponent
 *
 *	An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>HealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>HealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "<PROJECT_NAME>|Health")
	static U<PROJECT_NAME>HealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<U<PROJECT_NAME>HealthComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Health")
	void InitializeWithAbilitySystem(U<PROJECT_NAME>AbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Health")
	E<PROJECT_NAME>DeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "<PROJECT_NAME>|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > E<PROJECT_NAME>DeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:

	// Delegate fired when the health value has changed.
	UPROPERTY(BlueprintAssignable)
	F<PROJECT_NAME>Health_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed.
	UPROPERTY(BlueprintAssignable)
	F<PROJECT_NAME>Health_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	F<PROJECT_NAME>Health_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	F<PROJECT_NAME>Health_DeathEvent OnDeathFinished;

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void OnRep_DeathState(E<PROJECT_NAME>DeathState OldDeathState);

protected:

	// Ability system used by this component.
	UPROPERTY()
	U<PROJECT_NAME>AbilitySystemComponent* AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	const U<PROJECT_NAME>HealthSet* HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	E<PROJECT_NAME>DeathState DeathState;
};
