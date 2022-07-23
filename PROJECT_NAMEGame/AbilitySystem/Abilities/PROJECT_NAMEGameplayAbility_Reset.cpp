// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/<PROJECT_NAME>GameplayAbility_Reset.h"
#include "TimerManager.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "GameplayTagsManager.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Character/<PROJECT_NAME>Character.h"

U<PROJECT_NAME>GameplayAbility_Reset::U<PROJECT_NAME>GameplayAbility_Reset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateUObject(this, &ThisClass::DoneAddingNativeTags));
}

void U<PROJECT_NAME>GameplayAbility_Reset::DoneAddingNativeTags()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = F<PROJECT_NAME>GameplayTags::Get().GameplayEvent_RequestReset;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void U<PROJECT_NAME>GameplayAbility_Reset::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
		UE_LOG(Log<PROJECT_NAME>AbilitySystem, Error, TEXT("U<PROJECT_NAME>GameplayAbility_Reset::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	// Execute the reset from the character
	if (A<PROJECT_NAME>Character* <PROJECT_NAME>Char = Cast<A<PROJECT_NAME>Character>(CurrentActorInfo->AvatarActor.Get()))
	{
		<PROJECT_NAME>Char->Reset();
	}

	// Let others know a reset has occurred
	F<PROJECT_NAME>PlayerResetMessage Message;
	Message.OwnerPlayerState = CurrentActorInfo->OwnerActor.Get();
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(F<PROJECT_NAME>GameplayTags::Get().GameplayEvent_Reset, Message);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bReplicateEndAbility = true;
	const bool bWasCanceled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCanceled);
}
