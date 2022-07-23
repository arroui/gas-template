// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>UICameraManagerComponent.h"
#include "EngineUtils.h"
#include "Algo/Transform.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Slate/SceneViewport.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/HUD.h"
#include "Engine/Engine.h"

#include "<PROJECT_NAME>CameraMode.h"
#include "<PROJECT_NAME>PlayerCameraManager.h"

U<PROJECT_NAME>UICameraManagerComponent* U<PROJECT_NAME>UICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (A<PROJECT_NAME>PlayerCameraManager* PCCamera = Cast<A<PROJECT_NAME>PlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

U<PROJECT_NAME>UICameraManagerComponent::U<PROJECT_NAME>UICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void U<PROJECT_NAME>UICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void U<PROJECT_NAME>UICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<A<PROJECT_NAME>PlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool U<PROJECT_NAME>UICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void U<PROJECT_NAME>UICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void U<PROJECT_NAME>UICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}