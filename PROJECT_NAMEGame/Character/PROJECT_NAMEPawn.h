// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "Teams/<PROJECT_NAME>TeamAgentInterface.h"

#include "<PROJECT_NAME>Pawn.generated.h"

/**
 * A<PROJECT_NAME>Pawn
 */
UCLASS()
class A<PROJECT_NAME>Pawn : public AModularPawn, public I<PROJECT_NAME>TeamAgentInterface
{
	GENERATED_BODY()

public:

	A<PROJECT_NAME>Pawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~End of APawn interface

	//~I<PROJECT_NAME>TeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOn<PROJECT_NAME>TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of I<PROJECT_NAME>TeamAgentInterface interface

protected:
	// Called to determine what happens to the team ID when possession ends
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		return FGenericTeamId::NoTeam;
	}

private:
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FOn<PROJECT_NAME>TeamIndexChangedDelegate OnTeamChangedDelegate;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
