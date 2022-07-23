// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DebugCameraController.h"
#include "<PROJECT_NAME>DebugCameraController.generated.h"


/**
 * A<PROJECT_NAME>DebugCameraController
 *
 *	Used for controlling the debug camera when it is enabled via the cheat manager.
 */
UCLASS()
class A<PROJECT_NAME>DebugCameraController : public ADebugCameraController
{
	GENERATED_BODY()

public:

	A<PROJECT_NAME>DebugCameraController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void AddCheats(bool bForce) override;
};
