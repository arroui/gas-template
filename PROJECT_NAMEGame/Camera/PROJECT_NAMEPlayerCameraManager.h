// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "<PROJECT_NAME>PlayerCameraManager.generated.h"


#define <PROJECT_NAME_UPPER>_CAMERA_DEFAULT_FOV			(80.0f)
#define <PROJECT_NAME_UPPER>_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define <PROJECT_NAME_UPPER>_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class U<PROJECT_NAME>UICameraManagerComponent;

/**
 * A<PROJECT_NAME>PlayerCameraManager
 *
 *	The base player camera manager class used by this project.
 */
UCLASS(notplaceable, MinimalAPI)
class A<PROJECT_NAME>PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	A<PROJECT_NAME>PlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	U<PROJECT_NAME>UICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	U<PROJECT_NAME>UICameraManagerComponent* UICamera;
};
