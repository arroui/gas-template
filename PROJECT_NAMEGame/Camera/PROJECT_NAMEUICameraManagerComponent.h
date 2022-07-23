// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Camera/PlayerCameraManager.h"

#include "<PROJECT_NAME>UICameraManagerComponent.generated.h"

class APlayerController;
class A<PROJECT_NAME>PlayerCameraManager;
class AHUD;
class UCanvas;

UCLASS( Transient, Within=<PROJECT_NAME>PlayerCameraManager )
class U<PROJECT_NAME>UICameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static U<PROJECT_NAME>UICameraManagerComponent* GetComponent(APlayerController* PC);

public:
	U<PROJECT_NAME>UICameraManagerComponent();	
	virtual void InitializeComponent() override;

	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }
	AActor* GetViewTarget() const { return ViewTarget; }
	void SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());

	bool NeedsToUpdateViewTarget() const;
	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);

	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);

private:
	UPROPERTY(Transient)
	AActor* ViewTarget;
	
	UPROPERTY(Transient)
	bool bUpdatingViewTarget;
};
