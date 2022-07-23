// Copyright Epic Games, Inc. All Rights Reserved.


#include "<PROJECT_NAME>ContextEffectComponent.h"
#include "<PROJECT_NAME>ContextEffectsLibrary.h"
#include "<PROJECT_NAME>ContextEffectsSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"



// Sets default values for this component's properties
U<PROJECT_NAME>ContextEffectComponent::U<PROJECT_NAME>ContextEffectComponent()
{
	// Disable component tick, enable Auto Activate
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
	// ...
}


// Called when the game starts
void U<PROJECT_NAME>ContextEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentContexts.AppendTags(DefaultEffectContexts);
	CurrentContextEffectsLibraries = DefaultContextEffectsLibraries;

	// On Begin Play, Load and Add Context Effects pairings
	if (const UWorld* World = GetWorld())
	{
		if (U<PROJECT_NAME>ContextEffectsSubsystem* <PROJECT_NAME>ContextEffectsSubsystem = World->GetSubsystem<U<PROJECT_NAME>ContextEffectsSubsystem>())
		{
			<PROJECT_NAME>ContextEffectsSubsystem->LoadAndAddContextEffectsLibraries(GetOwner(), CurrentContextEffectsLibraries);
		}
	}
}

void U<PROJECT_NAME>ContextEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// On End PLay, remove unnecessary context effects pairings
	if (const UWorld* World = GetWorld())
	{
		if (U<PROJECT_NAME>ContextEffectsSubsystem* <PROJECT_NAME>ContextEffectsSubsystem = World->GetSubsystem<U<PROJECT_NAME>ContextEffectsSubsystem>())
		{
			<PROJECT_NAME>ContextEffectsSubsystem->UnloadAndRemoveContextEffectsLibraries(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}

// Implementation of Interface's AnimMotionEffect function
void U<PROJECT_NAME>ContextEffectComponent::AnimMotionEffect_Implementation(const FName Bone, const FGameplayTag MotionEffect, USceneComponent* StaticMeshComponent,
	const FVector LocationOffset, const FRotator RotationOffset, const UAnimSequenceBase* AnimationSequence,
	const bool bHitSuccess, const FHitResult HitResult, FGameplayTagContainer Contexts,
	FVector VFXScale, float AudioVolume, float AudioPitch)
{
	// Prep Components
	TArray<UAudioComponent*> AudioComponentsToAdd;
	TArray<UNiagaraComponent*> NiagaraComponentsToAdd;

	FGameplayTagContainer TotalContexts;

	// Aggregate contexts
	TotalContexts.AppendTags(Contexts);
	TotalContexts.AppendTags(CurrentContexts);

	// Check if converting Physical Surface Type to Context
	if (bConvertPhysicalSurfaceToContext)
	{
		// Get Phys Mat Type Pointer
		TWeakObjectPtr<UPhysicalMaterial> PhysicalSurfaceTypePtr = HitResult.PhysMaterial;

		// Check if pointer is okay
		if (PhysicalSurfaceTypePtr.IsValid())
		{
			// Get the Surface Type Pointer
			TEnumAsByte<EPhysicalSurface> PhysicalSurfaceType = PhysicalSurfaceTypePtr->SurfaceType;

			// If Settings are valid
			if (const U<PROJECT_NAME>ContextEffectsSettings* <PROJECT_NAME>ContextEffectsSettings = GetDefault<U<PROJECT_NAME>ContextEffectsSettings>())
			{
				// Convert Surface Type to known
				if (const FGameplayTag* SurfaceContextPtr = <PROJECT_NAME>ContextEffectsSettings->SurfaceTypeToContextMap.Find(PhysicalSurfaceType))
				{
					FGameplayTag SurfaceContext = *SurfaceContextPtr;

					TotalContexts.AddTag(SurfaceContext);
				}
			}
		}
	}

	// Cycle through Active Audio Components and cache
	for (UAudioComponent* ActiveAudioComponent : ActiveAudioComponents)
	{
		if (ActiveAudioComponent)
		{
			AudioComponentsToAdd.Add(ActiveAudioComponent);
		}
	}

	// Cycle through Active Niagara Components and cache
	for (UNiagaraComponent* ActiveNiagaraComponent : ActiveNiagaraComponents)
	{
		if (ActiveNiagaraComponent)
		{
			NiagaraComponentsToAdd.Add(ActiveNiagaraComponent);
		}
	}

	// Get World
	if (const UWorld* World = GetWorld())
	{
		// Get Subsystem
		if (U<PROJECT_NAME>ContextEffectsSubsystem* <PROJECT_NAME>ContextEffectsSubsystem = World->GetSubsystem<U<PROJECT_NAME>ContextEffectsSubsystem>())
		{
			// Set up Audio Components and Niagara
			TArray<UAudioComponent*> AudioComponents;
			TArray<UNiagaraComponent*> NiagaraComponents;

			// Spawn effects
			<PROJECT_NAME>ContextEffectsSubsystem->SpawnContextEffects(GetOwner(), StaticMeshComponent, Bone, 
				LocationOffset, RotationOffset, MotionEffect, TotalContexts,
				AudioComponents, NiagaraComponents, VFXScale, AudioVolume, AudioPitch);

			// Append resultant effects
			AudioComponentsToAdd.Append(AudioComponents);
			NiagaraComponentsToAdd.Append(NiagaraComponents);
		}
	}

	// Append Active Audio Components
	ActiveAudioComponents.Empty();
	ActiveAudioComponents.Append(AudioComponentsToAdd);

	// Append Active
	ActiveNiagaraComponents.Empty();
	ActiveNiagaraComponents.Append(NiagaraComponentsToAdd);

}

void U<PROJECT_NAME>ContextEffectComponent::UpdateEffectContexts(FGameplayTagContainer NewEffectContexts)
{
	// Reset and update
	CurrentContexts.Reset(NewEffectContexts.Num());
	CurrentContexts.AppendTags(NewEffectContexts);
}

void U<PROJECT_NAME>ContextEffectComponent::UpdateLibraries(
	TSet<TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>> NewContextEffectsLibraries)
{
	// Clear out existing Effects
	CurrentContextEffectsLibraries = NewContextEffectsLibraries;

	// Get World
	if (const UWorld* World = GetWorld())
	{
		// Get Subsystem
		if (U<PROJECT_NAME>ContextEffectsSubsystem* <PROJECT_NAME>ContextEffectsSubsystem = World->GetSubsystem<U<PROJECT_NAME>ContextEffectsSubsystem>())
		{
			// Load and Add Libraries to Subsystem                  
			<PROJECT_NAME>ContextEffectsSubsystem->LoadAndAddContextEffectsLibraries(GetOwner(), CurrentContextEffectsLibraries);
		}
	}
}
