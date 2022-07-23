// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PlayerCameraManager.h"
#include "<PROJECT_NAME>CameraComponent.h"
#include "Engine/Canvas.h"
#include "<PROJECT_NAME>UICameraManagerComponent.h"
#include "GameFramework/PlayerController.h"

static FName UICameraComponentName(TEXT("UICamera"));

A<PROJECT_NAME>PlayerCameraManager::A<PROJECT_NAME>PlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = <PROJECT_NAME_UPPER>_CAMERA_DEFAULT_FOV;
	ViewPitchMin = <PROJECT_NAME_UPPER>_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = <PROJECT_NAME_UPPER>_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<U<PROJECT_NAME>UICameraManagerComponent>(UICameraComponentName);
}

U<PROJECT_NAME>UICameraManagerComponent* A<PROJECT_NAME>PlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void A<PROJECT_NAME>PlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void A<PROJECT_NAME>PlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("<PROJECT_NAME>PlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const U<PROJECT_NAME>CameraComponent* CameraComponent = U<PROJECT_NAME>CameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}
