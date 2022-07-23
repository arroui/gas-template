// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "<PROJECT_NAME>DamageLogDebuggerComponent.generated.h"

struct F<PROJECT_NAME>VerbMessage;

struct FFrameDamageEntry
{
	int32 NumImpacts = 0;
	double SumDamage = 0.0;
	double TimeOfFirstHit = 0.0;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class U<PROJECT_NAME>DamageLogDebuggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>DamageLogDebuggerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	double SecondsBetweenDamageBeforeLogging = 1.0;

private:
	FGameplayMessageListenerHandle ListenerHandle;

	double LastDamageEntryTime = 0.0;
	TMap<int64, FFrameDamageEntry> DamageLog;

private:
	void OnDamageMessage(FGameplayTag Channel, const F<PROJECT_NAME>VerbMessage& Payload);
};
