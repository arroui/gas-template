// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"

#include "<PROJECT_NAME>TeamCreationComponent.generated.h"

class U<PROJECT_NAME>ExperienceDefinition;
class A<PROJECT_NAME>TeamPublicInfo;
class A<PROJECT_NAME>TeamPrivateInfo;
class A<PROJECT_NAME>PlayerState;
class AGameModeBase;
class APlayerController;
class U<PROJECT_NAME>TeamDisplayAsset;

UCLASS(Blueprintable)
class U<PROJECT_NAME>TeamCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>TeamCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	//~End of UObject interface

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

private:
	void OnExperienceLoaded(const U<PROJECT_NAME>ExperienceDefinition* Experience);

protected:
	// List of teams to create (id to display asset mapping, the display asset can be left unset if desired)
	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TMap<uint8, TObjectPtr<U<PROJECT_NAME>TeamDisplayAsset>> TeamsToCreate;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<A<PROJECT_NAME>TeamPublicInfo> PublicTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<A<PROJECT_NAME>TeamPrivateInfo> PrivateTeamInfoClass;

#if WITH_SERVER_CODE
protected:
	virtual void ServerCreateTeams();
	virtual void ServerAssignPlayersToTeams();

	/** Sets the team ID for the given player state. Spectator-only player states will be stripped of any team association. */
	virtual void ServerChooseTeamForPlayer(A<PROJECT_NAME>PlayerState* PS);

private:
	void OnPostLogin(AGameModeBase* GameMode, AController* NewPlayer);
	void ServerCreateTeam(int32 TeamId, U<PROJECT_NAME>TeamDisplayAsset* DisplayAsset);

	/** returns the Team ID with the fewest active players, or INDEX_NONE if there are no valid teams */
	int32 GetLeastPopulatedTeamID() const;
#endif
};
