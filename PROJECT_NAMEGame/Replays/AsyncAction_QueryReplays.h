// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "NetworkReplayStreaming.h"

#include "AsyncAction_QueryReplays.generated.h"

class AGameStateBase;
class UGameInstance;
class APlayerController;
class A<PROJECT_NAME>PlayerState;
class INetworkReplayStreamer;
class U<PROJECT_NAME>ReplayList;
class U<PROJECT_NAME>ReplayListEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQueryReplayAsyncDelegate, U<PROJECT_NAME>ReplayList*, Results);

/**
 * Watches for team changes in the specified player controller
 */
UCLASS()
class UAsyncAction_QueryReplays : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAsyncAction_QueryReplays(const FObjectInitializer& ObjectInitializer);

	// Watches for team changes in the specified player controller
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncAction_QueryReplays* QueryReplays(APlayerController* PlayerController);

	virtual void Activate() override;

public:
	// Called when the replay query completes
	UPROPERTY(BlueprintAssignable)
	FQueryReplayAsyncDelegate QueryComplete;

private:
	void OnEnumerateStreamsComplete(const FEnumerateStreamsResult& Result);

private:
	UPROPERTY()
	U<PROJECT_NAME>ReplayList* ResultList;

	TWeakObjectPtr<APlayerController> PlayerController;

	TSharedPtr<INetworkReplayStreamer> ReplayStreamer;
};
