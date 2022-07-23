// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CommonPlayerController.h"
#include "Camera/<PROJECT_NAME>CameraAssistInterface.h"
#include "Teams/<PROJECT_NAME>TeamAgentInterface.h"
#include "<PROJECT_NAME>PlayerController.generated.h"

class U<PROJECT_NAME>SettingsShared;
class A<PROJECT_NAME>PlayerState;
class U<PROJECT_NAME>AbilitySystemComponent;
class A<PROJECT_NAME>HUD;
class APawn;

/**
 * A<PROJECT_NAME>PlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class A<PROJECT_NAME>PlayerController : public ACommonPlayerController, public I<PROJECT_NAME>CameraAssistInterface, public I<PROJECT_NAME>TeamAgentInterface
{
	GENERATED_BODY()

public:

	A<PROJECT_NAME>PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|PlayerController")
	A<PROJECT_NAME>PlayerState* Get<PROJECT_NAME>PlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|PlayerController")
	U<PROJECT_NAME>AbilitySystemComponent* Get<PROJECT_NAME>AbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|PlayerController")
	A<PROJECT_NAME>HUD* Get<PROJECT_NAME>HUD() const;

	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AController interface
	virtual void OnUnPossess() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	//~End of APlayerController interface

	//~I<PROJECT_NAME>CameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of I<PROJECT_NAME>CameraAssistInterface interface

	//~ACommonPlayerController interface
	virtual void OnPossess(APawn* InPawn) override;
	//~End of ACommonPlayerController interface
	
	//~I<PROJECT_NAME>TeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOn<PROJECT_NAME>TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of I<PROJECT_NAME>TeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|Character")
	bool GetIsAutoRunning() const;

private:
	UPROPERTY()
	FOn<PROJECT_NAME>TeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	APlayerState* LastSeenPlayerState;

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

	//~APlayerController interface
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;

	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;

	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	void OnSettingsChanged(U<PROJECT_NAME>SettingsShared* Settings);
	
	void OnStartAutoRun();
	void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
};


// A player controller used for replay capture and playback
UCLASS()
class A<PROJECT_NAME>ReplayPlayerController : public A<PROJECT_NAME>PlayerController
{
	GENERATED_BODY()

	virtual void SetPlayer(UPlayer* InPlayer) override;
};
