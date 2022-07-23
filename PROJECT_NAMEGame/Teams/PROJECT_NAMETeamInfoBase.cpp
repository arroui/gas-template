// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamInfoBase.h"
#include "Net/UnrealNetwork.h"
#include "<PROJECT_NAME>TeamSubsystem.h"
#include "Engine/World.h"

A<PROJECT_NAME>TeamInfoBase::A<PROJECT_NAME>TeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void A<PROJECT_NAME>TeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void A<PROJECT_NAME>TeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void A<PROJECT_NAME>TeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<U<PROJECT_NAME>TeamSubsystem>();
		TeamSubsystem->UnregisterTeamInfo(this);
	}

	Super::EndPlay(EndPlayReason);
}

void A<PROJECT_NAME>TeamInfoBase::RegisterWithTeamSubsystem(U<PROJECT_NAME>TeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void A<PROJECT_NAME>TeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<U<PROJECT_NAME>TeamSubsystem>();
		RegisterWithTeamSubsystem(TeamSubsystem);
	}
}

void A<PROJECT_NAME>TeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void A<PROJECT_NAME>TeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}
