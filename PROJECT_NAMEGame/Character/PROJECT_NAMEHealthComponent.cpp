// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>HealthComponent.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "System/<PROJECT_NAME>AssetManager.h"
#include "System/<PROJECT_NAME>GameData.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayPrediction.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>HealthSet.h"
#include "Messages/<PROJECT_NAME>VerbMessage.h"
#include "Messages/<PROJECT_NAME>VerbMessageHelpers.h"
#include "NativeGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_<PROJECT_NAME>_Elimination_Message, "<PROJECT_NAME>.Elimination.Message");


U<PROJECT_NAME>HealthComponent::U<PROJECT_NAME>HealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = E<PROJECT_NAME>DeathState::NotDead;
}

void U<PROJECT_NAME>HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(U<PROJECT_NAME>HealthComponent, DeathState);
}

void U<PROJECT_NAME>HealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void U<PROJECT_NAME>HealthComponent::InitializeWithAbilitySystem(U<PROJECT_NAME>AbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<U<PROJECT_NAME>HealthSet>();
	if (!HealthSet)
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(U<PROJECT_NAME>HealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(U<PROJECT_NAME>HealthSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	AbilitySystemComponent->SetNumericAttributeBase(U<PROJECT_NAME>HealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);

	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetOwner(), UGameFrameworkComponentManager::NAME_HealthComponentReady);
}

void U<PROJECT_NAME>HealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void U<PROJECT_NAME>HealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		const F<PROJECT_NAME>GameplayTags& GameplayTags = F<PROJECT_NAME>GameplayTags::Get();

		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dead, 0);
	}
}

float U<PROJECT_NAME>HealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float U<PROJECT_NAME>HealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float U<PROJECT_NAME>HealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

    return nullptr;
}

void U<PROJECT_NAME>HealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void U<PROJECT_NAME>HealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void U<PROJECT_NAME>HealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;
			Payload.EventTag = F<PROJECT_NAME>GameplayTags::Get().GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// Send a standardized verb message that other systems can observe
		{
			F<PROJECT_NAME>VerbMessage Message;
			Message.Verb = TAG_<PROJECT_NAME>_Elimination_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = U<PROJECT_NAME>VerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
			Message.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
	}

#endif // #if WITH_SERVER_CODE
}

void U<PROJECT_NAME>HealthComponent::OnRep_DeathState(E<PROJECT_NAME>DeathState OldDeathState)
{
	const E<PROJECT_NAME>DeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		UE_LOG(Log<PROJECT_NAME>, Warning, TEXT("<PROJECT_NAME>HealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == E<PROJECT_NAME>DeathState::NotDead)
	{
		if (NewDeathState == E<PROJECT_NAME>DeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == E<PROJECT_NAME>DeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == E<PROJECT_NAME>DeathState::DeathStarted)
	{
		if (NewDeathState == E<PROJECT_NAME>DeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}

	ensureMsgf((DeathState == NewDeathState), TEXT("<PROJECT_NAME>HealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
}

void U<PROJECT_NAME>HealthComponent::StartDeath()
{
	if (DeathState != E<PROJECT_NAME>DeathState::NotDead)
	{
		return;
	}

	DeathState = E<PROJECT_NAME>DeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void U<PROJECT_NAME>HealthComponent::FinishDeath()
{
	if (DeathState != E<PROJECT_NAME>DeathState::DeathStarted)
	{
		return;
	}

	DeathState = E<PROJECT_NAME>DeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void U<PROJECT_NAME>HealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState == E<PROJECT_NAME>DeathState::NotDead) && AbilitySystemComponent)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = U<PROJECT_NAME>AssetManager::GetSubclass(U<PROJECT_NAME>GameData::Get().DamageGameplayEffect_SetByCaller);
		if (!DamageGE)
		{
			UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()), *U<PROJECT_NAME>GameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (!Spec)
		{
			UE_LOG(Log<PROJECT_NAME>, Error, TEXT("<PROJECT_NAME>HealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);

		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}

		const float DamageAmount = GetMaxHealth();

		Spec->SetSetByCallerMagnitude(F<PROJECT_NAME>GameplayTags::Get().SetByCaller_Damage, DamageAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}
