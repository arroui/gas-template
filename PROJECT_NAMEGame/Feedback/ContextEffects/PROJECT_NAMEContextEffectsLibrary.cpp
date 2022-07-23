// Copyright Epic Games, Inc. All Rights Reserved.


#include "<PROJECT_NAME>ContextEffectsLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "GameplayTagContainer.h"


void U<PROJECT_NAME>ContextEffectsLibrary::GetEffects(const FGameplayTag Effect, const FGameplayTagContainer Context, 
	TArray<USoundBase*>& Sounds, TArray<UNiagaraSystem*>& NiagaraSystems)
{
	// Make sure Effect is valid and Library is loaded
	if (Effect.IsValid() && Context.IsValid() && EffectsLoadState == EContextEffectsLibraryLoadState::Loaded)
	{
		// Loop through Context Effects
		for (const auto ActiveContextEffect : ActiveContextEffects)
		{
			// Make sure the Effect is an exact Tag Match and ensure the Context has all tags in the Effect (and neither or both are empty)
			if (Effect.MatchesTagExact(ActiveContextEffect->EffectTag)
				&& Context.HasAllExact(ActiveContextEffect->Context)
				&& (ActiveContextEffect->Context.IsEmpty() == Context.IsEmpty()))
			{
				// Get all Matching Sounds and Niagara Systems
				Sounds.Append(ActiveContextEffect->Sounds);
				NiagaraSystems.Append(ActiveContextEffect->NiagaraSystems);
			}
		}
	}
}

void U<PROJECT_NAME>ContextEffectsLibrary::LoadEffects()
{
	// Load Effects into Library if not currently loading
	if (EffectsLoadState != EContextEffectsLibraryLoadState::Loading)
	{
		// Set load state to loading
		EffectsLoadState = EContextEffectsLibraryLoadState::Loading;

		// Clear out any old Active Effects
		ActiveContextEffects.Empty();

		// Call internal loading function
		LoadEffectsInternal();
	}
}

EContextEffectsLibraryLoadState U<PROJECT_NAME>ContextEffectsLibrary::GetContextEffectsLibraryLoadState()
{
	// Return current Load State
	return EffectsLoadState;
}

void U<PROJECT_NAME>ContextEffectsLibrary::LoadEffectsInternal()
{
	// TODO Add Async Loading for Libraries

	// Copy data for async load
	TArray<F<PROJECT_NAME>ContextEffects> LocalContextEffects = ContextEffects;

	// Prepare Active Context Effects Array
	TArray<U<PROJECT_NAME>ActiveContextEffects*> ActiveContextEffectsArray;

	// Loop through Context Effects
	for (const F<PROJECT_NAME>ContextEffects& ContextEffect : LocalContextEffects)
	{
		// Make sure Tags are Valid
		if (ContextEffect.EffectTag.IsValid() && ContextEffect.Context.IsValid())
		{
			// Create new Active Context Effect
			U<PROJECT_NAME>ActiveContextEffects* NewActiveContextEffects = NewObject<U<PROJECT_NAME>ActiveContextEffects>(this);

			// Pass relevant tag data
			NewActiveContextEffects->EffectTag = ContextEffect.EffectTag;
			NewActiveContextEffects->Context = ContextEffect.Context;

			// Try to load and add Effects to New Active Context Effects
			for (const FSoftObjectPath& Effect : ContextEffect.Effects)
			{
				if (UObject* Object = Effect.TryLoad())
				{
					if (Object->IsA(USoundBase::StaticClass()))
					{
						if (USoundBase* SoundBase = Cast<USoundBase>(Object))
						{
							NewActiveContextEffects->Sounds.Add(SoundBase);
						}
					}
					else if (Object->IsA(UNiagaraSystem::StaticClass()))
					{
						if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(Object))
						{
							NewActiveContextEffects->NiagaraSystems.Add(NiagaraSystem);
						}
					}
				}
			}

			// Add New Active Context to the Active Context Effects Array
			ActiveContextEffectsArray.Add(NewActiveContextEffects);
		}
	}

	// TODO Call Load Complete after Async Load
	// Mark loading complete
	this-><PROJECT_NAME>ContextEffectLibraryLoadingComplete(ActiveContextEffectsArray);
}

void U<PROJECT_NAME>ContextEffectsLibrary::<PROJECT_NAME>ContextEffectLibraryLoadingComplete(
	TArray<U<PROJECT_NAME>ActiveContextEffects*> <PROJECT_NAME>ActiveContextEffects)
{
	// Flag data as loaded
	EffectsLoadState = EContextEffectsLibraryLoadState::Loaded;

	// Append incoming Context Effects Array to current list of Active Context Effects
	ActiveContextEffects.Append(<PROJECT_NAME>ActiveContextEffects);
}
