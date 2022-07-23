// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PlayerState.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "Net/UnrealNetwork.h"
#include "<PROJECT_NAME>PlayerController.h"
#include "Character/<PROJECT_NAME>PawnExtensionComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySet.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>HealthSet.h"
#include "AbilitySystem/Attributes/<PROJECT_NAME>CombatSet.h"
#include "Character/<PROJECT_NAME>PawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameplayMessageSubsystem.h"

//@TODO: Would like to isolate this a bit better to get the pawn data in here without this having to know about other stuff
#include "GameModes/<PROJECT_NAME>GameMode.h"
#include "GameModes/<PROJECT_NAME>ExperienceManagerComponent.h"

const FName A<PROJECT_NAME>PlayerState::NAME_<PROJECT_NAME>AbilityReady("<PROJECT_NAME>AbilitiesReady");

A<PROJECT_NAME>PlayerState::A<PROJECT_NAME>PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(E<PROJECT_NAME>PlayerConnectionType::Player)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<U<PROJECT_NAME>AbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<U<PROJECT_NAME>HealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<U<PROJECT_NAME>CombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;

	MyTeamID = FGenericTeamId::NoTeam;
	MySquadID = INDEX_NONE;
}

void A<PROJECT_NAME>PlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void A<PROJECT_NAME>PlayerState::Reset()
{
	Super::Reset();
}

void A<PROJECT_NAME>PlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckPawnReadyToInitialize();
	}
}

void A<PROJECT_NAME>PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void A<PROJECT_NAME>PlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
		case E<PROJECT_NAME>PlayerConnectionType::Player:
		case E<PROJECT_NAME>PlayerConnectionType::InactivePlayer:
			//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
			// (e.g., for long running servers where they might build up if lots of players cycle through)
			bDestroyDeactivatedPlayerState = true;
			break;
		default:
			bDestroyDeactivatedPlayerState = true;
			break;
	}
	
	SetPlayerConnectionType(E<PROJECT_NAME>PlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void A<PROJECT_NAME>PlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == E<PROJECT_NAME>PlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(E<PROJECT_NAME>PlayerConnectionType::Player);
	}
}

void A<PROJECT_NAME>PlayerState::OnExperienceLoaded(const U<PROJECT_NAME>ExperienceDefinition* /*CurrentExperience*/)
{
	if (A<PROJECT_NAME>GameMode* <PROJECT_NAME>GameMode = GetWorld()->GetAuthGameMode<A<PROJECT_NAME>GameMode>())
	{
		if (const U<PROJECT_NAME>PawnData* NewPawnData = <PROJECT_NAME>GameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>, Error, TEXT("A<PROJECT_NAME>PlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void A<PROJECT_NAME>PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);
}

A<PROJECT_NAME>PlayerController* A<PROJECT_NAME>PlayerState::Get<PROJECT_NAME>PlayerController() const
{
	return Cast<A<PROJECT_NAME>PlayerController>(GetOwner());
}

UAbilitySystemComponent* A<PROJECT_NAME>PlayerState::GetAbilitySystemComponent() const
{
	return Get<PROJECT_NAME>AbilitySystemComponent();
}

void A<PROJECT_NAME>PlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	if (GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOn<PROJECT_NAME>ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void A<PROJECT_NAME>PlayerState::SetPawnData(const U<PROJECT_NAME>PawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const U<PROJECT_NAME>AbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_<PROJECT_NAME>AbilityReady);
	
	ForceNetUpdate();
}

void A<PROJECT_NAME>PlayerState::OnRep_PawnData()
{
}

void A<PROJECT_NAME>PlayerState::SetPlayerConnectionType(E<PROJECT_NAME>PlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void A<PROJECT_NAME>PlayerState::SetSquadID(int32 NewSquadId)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		MySquadID = NewSquadId;
	}
}

void A<PROJECT_NAME>PlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyTeamID, this);
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
	}
}

FGenericTeamId A<PROJECT_NAME>PlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}

FOn<PROJECT_NAME>TeamIndexChangedDelegate* A<PROJECT_NAME>PlayerState::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void A<PROJECT_NAME>PlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void A<PROJECT_NAME>PlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
}

void A<PROJECT_NAME>PlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void A<PROJECT_NAME>PlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 A<PROJECT_NAME>PlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool A<PROJECT_NAME>PlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void A<PROJECT_NAME>PlayerState::ClientBroadcastMessage_Implementation(const F<PROJECT_NAME>VerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}
