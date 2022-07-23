// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "<PROJECT_NAME>TeamInfoBase.h"

#include "<PROJECT_NAME>TeamPublicInfo.generated.h"

class U<PROJECT_NAME>TeamDisplayAsset;

UCLASS()
class A<PROJECT_NAME>TeamPublicInfo : public A<PROJECT_NAME>TeamInfoBase
{
	GENERATED_BODY()

	friend U<PROJECT_NAME>TeamCreationComponent;

public:
	A<PROJECT_NAME>TeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	U<PROJECT_NAME>TeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<U<PROJECT_NAME>TeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<U<PROJECT_NAME>TeamDisplayAsset> TeamDisplayAsset;
};
