// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"

#include "<PROJECT_NAME>UIManagerSubsystem.generated.h"

UCLASS()
class U<PROJECT_NAME>UIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>UIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();
	
	FTSTicker::FDelegateHandle TickHandle;
};