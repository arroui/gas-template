// Copyright Epic Games, Inc. All Rights Reserved.

#include "IndicatorLibrary.h"
#include "<PROJECT_NAME>IndicatorManagerComponent.h"

UIndicatorLibrary::UIndicatorLibrary()
{
}

U<PROJECT_NAME>IndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return U<PROJECT_NAME>IndicatorManagerComponent::GetComponent(Controller);
}
