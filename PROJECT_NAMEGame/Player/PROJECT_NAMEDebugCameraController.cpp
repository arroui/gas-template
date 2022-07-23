// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>DebugCameraController.h"
#include "<PROJECT_NAME>CheatManager.h"


A<PROJECT_NAME>DebugCameraController::A<PROJECT_NAME>DebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as <PROJECT_NAME>PlayerController to allow toggling the debug camera through cheats.
	CheatClass = U<PROJECT_NAME>CheatManager::StaticClass();
}

void A<PROJECT_NAME>DebugCameraController::AddCheats(bool bForce)
{
	// Mirrors <PROJECT_NAME>PlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}
