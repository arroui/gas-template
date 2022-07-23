// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameplayAbility_Death.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "Character/<PROJECT_NAME>HealthComponent.h"
#include "GameplayTagsManager.h"

U<PROJECT_NAME>GameplayAbility_Death::U<PROJECT_NAME>GameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateUObject(this, &ThisClass::DoneAddingNativeTags));
}

void U<PROJECT_NAME>GameplayAbility_Death::DoneAddingNativeTags()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = F<PROJECT_NAME>GameplayTags::Get().GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void U<PROJECT_NAME>GameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = CastChecked<U<PROJECT_NAME>AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(F<PROJECT_NAME>GameplayTags::Get().Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting.
	<PROJECT_NAME>ASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (!ChangeActivationGroup(E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Blocking))
	{
		UE_LOG(Log<PROJECT_NAME>AbilitySystem, Error, TEXT("U<PROJECT_NAME>GameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void U<PROJECT_NAME>GameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void U<PROJECT_NAME>GameplayAbility_Death::StartDeath()
{
	if (U<PROJECT_NAME>HealthComponent* HealthComponent = U<PROJECT_NAME>HealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == E<PROJECT_NAME>DeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void U<PROJECT_NAME>GameplayAbility_Death::FinishDeath()
{
	if (U<PROJECT_NAME>HealthComponent* HealthComponent = U<PROJECT_NAME>HealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == E<PROJECT_NAME>DeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
