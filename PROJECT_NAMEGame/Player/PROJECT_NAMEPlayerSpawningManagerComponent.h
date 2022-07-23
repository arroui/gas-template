// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"
#include "GameFramework/OnlineReplStructs.h"

#include "<PROJECT_NAME>PlayerSpawningManagerComponent.generated.h"

class AController;
class APlayerController;
class APlayerState;
class APlayerStart;
class A<PROJECT_NAME>PlayerStart;
class AActor;

/**
 * @class U<PROJECT_NAME>PlayerSpawningManagerComponent
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>PlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>PlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);

	/** UActorComponent */
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** ~UActorComponent */

protected:
	// Utility
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<A<PROJECT_NAME>PlayerStart*>& FoundStartPoints) const;
	
	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<A<PROJECT_NAME>PlayerStart*>& PlayerStarts) { return nullptr; }
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) { }

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName=OnFinishRestartPlayer))
	void K2_OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);

private:

	/** We proxy these calls from A<PROJECT_NAME>GameMode, to this component so that each experience can more easily customize the respawn system they want. */
	AActor* ChoosePlayerStart(AController* Player);
	bool ControllerCanRestart(AController* Player);
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);
	friend class A<PROJECT_NAME>GameMode;
	/** ~A<PROJECT_NAME>GameMode */

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<A<PROJECT_NAME>PlayerStart>> CachedPlayerStarts;

private:
	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
};
