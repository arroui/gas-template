// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "System/GameplayTagStack.h"

#include "<PROJECT_NAME>TeamInfoBase.generated.h"

class U<PROJECT_NAME>TeamCreationComponent;
class U<PROJECT_NAME>TeamSubsystem;

UCLASS(Abstract)
class A<PROJECT_NAME>TeamInfoBase : public AInfo
{
	GENERATED_BODY()

public:
	A<PROJECT_NAME>TeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	int32 GetTeamId() const { return TeamId; }

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

protected:
	virtual void RegisterWithTeamSubsystem(U<PROJECT_NAME>TeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

private:
	void SetTeamId(int32 NewTeamId);

	UFUNCTION()
	void OnRep_TeamId();

public:
	friend U<PROJECT_NAME>TeamCreationComponent;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamId)
	int32 TeamId;
};
