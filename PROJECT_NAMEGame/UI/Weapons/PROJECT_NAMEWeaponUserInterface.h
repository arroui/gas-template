// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "<PROJECT_NAME>WeaponUserInterface.generated.h"

class U<PROJECT_NAME>WeaponInstance;

UCLASS()
class U<PROJECT_NAME>WeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>WeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(U<PROJECT_NAME>WeaponInstance* OldWeapon, U<PROJECT_NAME>WeaponInstance* NewWeapon);

private:
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<U<PROJECT_NAME>WeaponInstance> CurrentInstance;
};
