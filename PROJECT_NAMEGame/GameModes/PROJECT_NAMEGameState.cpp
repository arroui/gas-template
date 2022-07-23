// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameState.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "GameModes/<PROJECT_NAME>ExperienceManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameplayMessageSubsystem.h"

extern ENGINE_API float GAverageFPS;


A<PROJECT_NAME>GameState::A<PROJECT_NAME>GameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<U<PROJECT_NAME>AbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<U<PROJECT_NAME>ExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));

	ServerFPS = 0.0f;
}

void A<PROJECT_NAME>GameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void A<PROJECT_NAME>GameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* A<PROJECT_NAME>GameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void A<PROJECT_NAME>GameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void A<PROJECT_NAME>GameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void A<PROJECT_NAME>GameState::RemovePlayerState(APlayerState* PlayerState)
{
	//@TODO: This isn't getting called right now (only the 'rich' AGameMode uses it, not AGameModeBase)
	// Need to at least comment the engine code, and possibly move things around
	Super::RemovePlayerState(PlayerState);
}

void A<PROJECT_NAME>GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void A<PROJECT_NAME>GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

void A<PROJECT_NAME>GameState::MulticastMessageToClients_Implementation(const F<PROJECT_NAME>VerbMessage Message)
{
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

void A<PROJECT_NAME>GameState::MulticastReliableMessageToClients_Implementation(const F<PROJECT_NAME>VerbMessage Message)
{
	MulticastMessageToClients_Implementation(Message);
}