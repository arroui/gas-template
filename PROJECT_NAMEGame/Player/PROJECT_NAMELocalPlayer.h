// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Teams/<PROJECT_NAME>TeamAgentInterface.h"
#include "<PROJECT_NAME>LocalPlayer.generated.h"

class U<PROJECT_NAME>SettingsLocal;
class U<PROJECT_NAME>SettingsShared;
class UInputMappingContext;

/**
 * U<PROJECT_NAME>LocalPlayer
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>LocalPlayer : public UCommonLocalPlayer, public I<PROJECT_NAME>TeamAgentInterface
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>LocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	//~UPlayer interface
	virtual void SwitchController(class APlayerController* PC) override;
	//~End of UPlayer interface

	//~ULocalPlayer interface
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	virtual void InitOnlineSession() override;
	//~End of ULocalPlayer interface

	//~I<PROJECT_NAME>TeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOn<PROJECT_NAME>TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of I<PROJECT_NAME>TeamAgentInterface interface

public:
	UFUNCTION()
	U<PROJECT_NAME>SettingsLocal* GetLocalSettings() const;

	UFUNCTION()
	U<PROJECT_NAME>SettingsShared* GetSharedSettings() const;

protected:
	void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);
	
	UFUNCTION()
	void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

private:
	void OnPlayerControllerChanged(APlayerController* NewController);

	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	UPROPERTY(Transient)
	mutable U<PROJECT_NAME>SettingsShared* SharedSettings;

	UPROPERTY(Transient)
	mutable const UInputMappingContext* InputMappingContext;

	UPROPERTY()
	FOn<PROJECT_NAME>TeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
