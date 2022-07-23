// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>EquipmentInstance.h"
#include "<PROJECT_NAME>EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

U<PROJECT_NAME>EquipmentInstance::U<PROJECT_NAME>EquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* U<PROJECT_NAME>EquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

void U<PROJECT_NAME>EquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

APawn* U<PROJECT_NAME>EquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* U<PROJECT_NAME>EquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void U<PROJECT_NAME>EquipmentInstance::SpawnEquipmentActors(const TArray<F<PROJECT_NAME>EquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const F<PROJECT_NAME>EquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void U<PROJECT_NAME>EquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void U<PROJECT_NAME>EquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void U<PROJECT_NAME>EquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void U<PROJECT_NAME>EquipmentInstance::OnRep_Instigator()
{
}
