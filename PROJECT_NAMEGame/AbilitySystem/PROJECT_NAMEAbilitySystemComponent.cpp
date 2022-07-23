// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AbilitySystemComponent.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "System/<PROJECT_NAME>GameData.h"
#include "System/<PROJECT_NAME>AssetManager.h"
#include "<PROJECT_NAME>GlobalAbilitySystem.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/<PROJECT_NAME>GameplayAbility.h"
#include "Animation/<PROJECT_NAME>AnimInstance.h"
#include "AbilitySystem/<PROJECT_NAME>AbilityTagRelationshipMapping.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

U<PROJECT_NAME>AbilitySystemComponent::U<PROJECT_NAME>AbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

void U<PROJECT_NAME>AbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (U<PROJECT_NAME>GlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<U<PROJECT_NAME>GlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// Notify all abilities that a new pawn avatar has been set
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityCDO = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilitySpec.Ability);

			if (<PROJECT_NAME>AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : Instances)
				{
					U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityInstance = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilityInstance);
					<PROJECT_NAME>AbilityInstance->OnPawnAvatarSet();
				}
			}
			else
			{
				<PROJECT_NAME>AbilityCDO->OnPawnAvatarSet();
			}
		}

		// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
		if (U<PROJECT_NAME>GlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<U<PROJECT_NAME>GlobalAbilitySystem>(GetWorld()))
		{
			GlobalAbilitySystem->RegisterASC(this);
		}

		if (U<PROJECT_NAME>AnimInstance* <PROJECT_NAME>AnimInst = Cast<U<PROJECT_NAME>AnimInstance>(ActorInfo->GetAnimInstance()))
		{
			<PROJECT_NAME>AnimInst->InitializeWithAbilitySystem(this);
		}

		TryActivateAbilitiesOnSpawn();
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityCDO = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilitySpec.Ability);
		<PROJECT_NAME>AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityCDO = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilitySpec.Ability);

		if (<PROJECT_NAME>AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// Cancel all the spawned instances, not the CDO.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityInstance = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilityInstance);

				if (ShouldCancelFunc(<PROJECT_NAME>AbilityInstance, AbilitySpec.Handle))
				{
					if (<PROJECT_NAME>AbilityInstance->CanBeCanceled())
					{
						<PROJECT_NAME>AbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), <PROJECT_NAME>AbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(Log<PROJECT_NAME>AbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *<PROJECT_NAME>AbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// Cancel the non-instanced ability CDO.
			if (ShouldCancelFunc(<PROJECT_NAME>AbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled.
				check(<PROJECT_NAME>AbilityCDO->CanBeCanceled());
				<PROJECT_NAME>AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
	TShouldCancelAbilityFunc ShouldCancelFunc = [this](const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability, FGameplayAbilitySpecHandle Handle)
	{
		const E<PROJECT_NAME>AbilityActivationPolicy ActivationPolicy = <PROJECT_NAME>Ability->GetActivationPolicy();
		return ((ActivationPolicy == E<PROJECT_NAME>AbilityActivationPolicy::OnInputTriggered) || (ActivationPolicy == E<PROJECT_NAME>AbilityActivationPolicy::WhileInputActive));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void U<PROJECT_NAME>AbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//@TODO: See if we can use FScopedServerAbilityRPCBatcher ScopedRPCBatcher in some of these loops

	//
	// Process all abilities that activate when the input is held.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityCDO = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilitySpec->Ability);

				if (<PROJECT_NAME>AbilityCDO->GetActivationPolicy() == E<PROJECT_NAME>AbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//
	// Process all abilities that had their input pressed this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>AbilityCDO = CastChecked<U<PROJECT_NAME>GameplayAbility>(AbilitySpec->Ability);

					if (<PROJECT_NAME>AbilityCDO->GetActivationPolicy() == E<PROJECT_NAME>AbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	//
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void U<PROJECT_NAME>AbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void U<PROJECT_NAME>AbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability = CastChecked<U<PROJECT_NAME>GameplayAbility>(Ability);

	AddAbilityToActivationGroup(<PROJECT_NAME>Ability->GetActivationGroup(), <PROJECT_NAME>Ability);
}

void U<PROJECT_NAME>AbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	HandleAbilityFailed(Ability, FailureReason);
}

void U<PROJECT_NAME>AbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability = CastChecked<U<PROJECT_NAME>GameplayAbility>(Ability);

	RemoveAbilityFromActivationGroup(<PROJECT_NAME>Ability->GetActivationGroup(), <PROJECT_NAME>Ability);
}

void U<PROJECT_NAME>AbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		// Use the mapping to expand the ability tags into block and cancel tag
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);

	//@TODO: Apply any special logic like blocking input or movement
}

void U<PROJECT_NAME>AbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);

	//@TODO: Apply any special logic like blocking input or movement
}

void U<PROJECT_NAME>AbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::SetTagRelationshipMapping(U<PROJECT_NAME>AbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void U<PROJECT_NAME>AbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void U<PROJECT_NAME>AbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	//UE_LOG(Log<PROJECT_NAME>AbilitySystem, Warning, TEXT("Ability %s failed to activate (tags: %s)"), *GetPathNameSafe(Ability), *FailureReason.ToString());

	if (const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability = Cast<const U<PROJECT_NAME>GameplayAbility>(Ability))
	{
		<PROJECT_NAME>Ability->OnAbilityFailedToActivate(FailureReason);
	}	
}

bool U<PROJECT_NAME>AbilitySystemComponent::IsActivationGroupBlocked(E<PROJECT_NAME>AbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case E<PROJECT_NAME>AbilityActivationGroup::Independent:
		// Independent abilities are never blocked.
		bBlocked = false;
		break;

	case E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Replaceable:
	case E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Blocking:
		// Exclusive abilities can activate if nothing is blocking.
		bBlocked = (ActivationGroupCounts[(uint8)E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

void U<PROJECT_NAME>AbilitySystemComponent::AddAbilityToActivationGroup(E<PROJECT_NAME>AbilityActivationGroup Group, U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability)
{
	check(<PROJECT_NAME>Ability);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	const bool bReplicateCancelAbility = false;

	switch (Group)
	{
	case E<PROJECT_NAME>AbilityActivationGroup::Independent:
		// Independent abilities do not cancel any other abilities.
		break;

	case E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Replaceable:
	case E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Replaceable, <PROJECT_NAME>Ability, bReplicateCancelAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)E<PROJECT_NAME>AbilityActivationGroup::Exclusive_Blocking];
	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(Log<PROJECT_NAME>AbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

void U<PROJECT_NAME>AbilitySystemComponent::RemoveAbilityFromActivationGroup(E<PROJECT_NAME>AbilityActivationGroup Group, U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability)
{
	check(<PROJECT_NAME>Ability);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

void U<PROJECT_NAME>AbilitySystemComponent::CancelActivationGroupAbilities(E<PROJECT_NAME>AbilityActivationGroup Group, U<PROJECT_NAME>GameplayAbility* Ignore<PROJECT_NAME>Ability, bool bReplicateCancelAbility)
{
	TShouldCancelAbilityFunc ShouldCancelFunc = [this, Group, Ignore<PROJECT_NAME>Ability](const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability, FGameplayAbilitySpecHandle Handle)
	{
		return ((<PROJECT_NAME>Ability->GetActivationGroup() == Group) && (<PROJECT_NAME>Ability != Ignore<PROJECT_NAME>Ability));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void U<PROJECT_NAME>AbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = U<PROJECT_NAME>AssetManager::GetSubclass(U<PROJECT_NAME>GameData::Get().DynamicTagGameplayEffect);
	if (!DynamicTagGE)
	{
		UE_LOG(Log<PROJECT_NAME>AbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect [%s]."), *U<PROJECT_NAME>GameData::Get().DynamicTagGameplayEffect.GetAssetName());
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(Log<PROJECT_NAME>AbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."), *GetNameSafe(DynamicTagGE));
		return;
	}

	Spec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*Spec);
}

void U<PROJECT_NAME>AbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = U<PROJECT_NAME>AssetManager::GetSubclass(U<PROJECT_NAME>GameData::Get().DynamicTagGameplayEffect);
	if (!DynamicTagGE)
	{
		UE_LOG(Log<PROJECT_NAME>AbilitySystem, Warning, TEXT("RemoveDynamicTagGameplayEffect: Unable to find gameplay effect [%s]."), *U<PROJECT_NAME>GameData::Get().DynamicTagGameplayEffect.GetAssetName());
		return;
	}

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	Query.EffectDefinition = DynamicTagGE;

	RemoveActiveEffects(Query);
}

void U<PROJECT_NAME>AbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}
