// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "Teams/<PROJECT_NAME>TeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "<PROJECT_NAME>PlayerBotController.generated.h"

/**
 * A<PROJECT_NAME>PlayerBotController
 *
 *	The controller class used by player bots in this project.
 */
UCLASS(Blueprintable)
class A<PROJECT_NAME>PlayerBotController : public AModularAIController, public I<PROJECT_NAME>TeamAgentInterface
{
	GENERATED_BODY()

public:
	A<PROJECT_NAME>PlayerBotController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~I<PROJECT_NAME>TeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOn<PROJECT_NAME>TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~End of I<PROJECT_NAME>TeamAgentInterface interface

	// Attempts to restart this controller (e.g., to respawn it)
	void ServerRestartController();

	//Update Team Attitude for the AI
	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME> AI Player Controller")
	void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);

	virtual void OnUnPossess() override;


private:
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:	
	//~AController interface
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

private:
	UPROPERTY()
	FOn<PROJECT_NAME>TeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	APlayerState* LastSeenPlayerState;
};
