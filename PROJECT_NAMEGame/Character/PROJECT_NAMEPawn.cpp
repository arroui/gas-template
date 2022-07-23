// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>Pawn.h"
#include "Net/UnrealNetwork.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"

A<PROJECT_NAME>Pawn::A<PROJECT_NAME>Pawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void A<PROJECT_NAME>Pawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID);
}

void A<PROJECT_NAME>Pawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void A<PROJECT_NAME>Pawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void A<PROJECT_NAME>Pawn::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	// Grab the current team ID and listen for future changes
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void A<PROJECT_NAME>Pawn::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void A<PROJECT_NAME>Pawn::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("You can't set the team ID on a pawn (%s) except on the authority"), *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("You can't set the team ID on a possessed pawn (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId A<PROJECT_NAME>Pawn::GetGenericTeamId() const
{
	return MyTeamID;
}

FOn<PROJECT_NAME>TeamIndexChangedDelegate* A<PROJECT_NAME>Pawn::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void A<PROJECT_NAME>Pawn::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void A<PROJECT_NAME>Pawn::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}
