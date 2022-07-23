// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>EquipmentDefinition.h"
#include "<PROJECT_NAME>EquipmentInstance.h"

U<PROJECT_NAME>EquipmentDefinition::U<PROJECT_NAME>EquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = U<PROJECT_NAME>EquipmentInstance::StaticClass();
}
