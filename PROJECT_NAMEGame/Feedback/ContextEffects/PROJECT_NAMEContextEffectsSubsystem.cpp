// Copyright Epic Games, Inc. All Rights Reserved.


#include "<PROJECT_NAME>ContextEffectsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "<PROJECT_NAME>ContextEffectsLibrary.h"

void U<PROJECT_NAME>ContextEffectsSubsystem::SpawnContextEffects(
	const AActor* SpawningActor
	, USceneComponent* AttachToComponent
	, const FName AttachPoint
	, const FVector LocationOffset
	, const FRotator RotationOffset
	, FGameplayTag Effect
	, FGameplayTagContainer Contexts
	, TArray<UAudioComponent*>& AudioOut
	, TArray<UNiagaraComponent*>& NiagaraOut
	, FVector VFXScale
	, float AudioVolume
	, float AudioPitch)
{
	// First determine if this Actor has a matching Set of Libraries
	if (U<PROJECT_NAME>ContextEffectsSet** EffectsLibrariesSetPtr = ActiveActorEffectsMap.Find(SpawningActor))
	{
		// Validate the pointers from the Map Find
		if (U<PROJECT_NAME>ContextEffectsSet* EffectsLibraries = *EffectsLibrariesSetPtr)
		{
			// Prepare Arrays for Sounds and Niagara Systems
			TArray<USoundBase*> TotalSounds;
			TArray<UNiagaraSystem*> TotalNiagaraSystems;

			// Cycle through Effect Libraries
			for (U<PROJECT_NAME>ContextEffectsLibrary* EffectLibrary : EffectsLibraries-><PROJECT_NAME>ContextEffectsLibraries)
			{
				// Check if the Effect Library is valid and data Loaded
				if (EffectLibrary && EffectLibrary->GetContextEffectsLibraryLoadState() == EContextEffectsLibraryLoadState::Loaded)
				{
					// Set up local list of Sounds and Niagara Systems
					TArray<USoundBase*> Sounds;
					TArray<UNiagaraSystem*> NiagaraSystems;

					// Get Sounds and Niagara Systems
					EffectLibrary->GetEffects(Effect, Contexts, Sounds, NiagaraSystems);

					// Append to accumulating array
					TotalSounds.Append(Sounds);
					TotalNiagaraSystems.Append(NiagaraSystems);
				}
				else if (EffectLibrary && EffectLibrary->GetContextEffectsLibraryLoadState() == EContextEffectsLibraryLoadState::Unloaded)
				{
					// Else load effects
					EffectLibrary->LoadEffects();
				}
			}

			// Cycle through found Sounds
			for (USoundBase* Sound : TotalSounds)
			{
				// Spawn Sounds Attached, add Audio Component to List of ACs
				UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, AttachToComponent, AttachPoint, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset,
					false, AudioVolume, AudioPitch, 0.0f, nullptr, nullptr, true);

				AudioOut.Add(AudioComponent);
			}

			// Cycle through found Niagara Systems
			for (UNiagaraSystem* NiagaraSystem : TotalNiagaraSystems)
			{
				// Spawn Niagara Systems Attached, add Niagara Component to List of NCs
				UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, AttachToComponent, AttachPoint, LocationOffset,
					RotationOffset, VFXScale, EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None, true, true);

				NiagaraOut.Add(NiagaraComponent);
			}
		}
	}
}

bool U<PROJECT_NAME>ContextEffectsSubsystem::GetContextFromSurfaceType(
	TEnumAsByte<EPhysicalSurface> PhysicalSurface, FGameplayTag& Context)
{
	// Get Project Settings
	if (const U<PROJECT_NAME>ContextEffectsSettings* ProjectSettings = GetDefault<U<PROJECT_NAME>ContextEffectsSettings>())
	{
		// Find which Gameplay Tag the Surface Type is mapped to
		if (const FGameplayTag* GameplayTagPtr = ProjectSettings->SurfaceTypeToContextMap.Find(PhysicalSurface))
		{
			Context = *GameplayTagPtr;
		}
	}

	// Return true if Context is Valid
	return Context.IsValid();
}

void U<PROJECT_NAME>ContextEffectsSubsystem::LoadAndAddContextEffectsLibraries(AActor* OwningActor,
	TSet<TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>> ContextEffectsLibraries)
{
	// Early out if Owning Actor is invalid or if the associated Libraries is 0 (or less)
	if (OwningActor == nullptr || ContextEffectsLibraries.Num() <= 0)
	{
		return;
	}

	// Create new Context Effect Set
	U<PROJECT_NAME>ContextEffectsSet* EffectsLibrariesSet = NewObject<U<PROJECT_NAME>ContextEffectsSet>(this);

	// Cycle through Libraries getting Soft Obj Refs
	for (const TSoftObjectPtr<U<PROJECT_NAME>ContextEffectsLibrary>& ContextEffectSoftObj : ContextEffectsLibraries)
	{
		// Load Library Assets from Soft Obj refs
		// TODO Support Async Loading of Asset Data
		if (U<PROJECT_NAME>ContextEffectsLibrary* EffectsLibrary = ContextEffectSoftObj.LoadSynchronous())
		{
			// Call load on valid Libraries
			EffectsLibrary->LoadEffects();

			// Add new library to Set
			EffectsLibrariesSet-><PROJECT_NAME>ContextEffectsLibraries.Add(EffectsLibrary);
		}
	}

	// Update Active Actor Effects Map
	ActiveActorEffectsMap.Emplace(OwningActor, EffectsLibrariesSet);
}

void U<PROJECT_NAME>ContextEffectsSubsystem::UnloadAndRemoveContextEffectsLibraries(AActor* OwningActor)
{
	// Early out if Owning Actor is invalid
	if (OwningActor == nullptr)
	{
		return;
	}

	// Remove ref from Active Actor/Effects Set Map
	ActiveActorEffectsMap.Remove(OwningActor);
}
