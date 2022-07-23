// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ControllerComponent_CharacterParts.h"
#include "<PROJECT_NAME>PawnComponent_CharacterParts.h"
#include "GameFramework/Controller.h"
#include "<PROJECT_NAME>CosmeticDeveloperSettings.h"
#include "GameFramework/CheatManager.h"

//////////////////////////////////////////////////////////////////////

U<PROJECT_NAME>ControllerComponent_CharacterParts::U<PROJECT_NAME>ControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	// Listen for pawn possession changed events
	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = GetPawn<APawn>())
			{
				OnPossessedPawnChanged(nullptr, ControlledPawn);
			}
		}

		ApplyDeveloperSettings();
	}
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

U<PROJECT_NAME>PawnComponent_CharacterParts* U<PROJECT_NAME>ControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<U<PROJECT_NAME>PawnComponent_CharacterParts>();
	}
	return nullptr;
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::AddCharacterPart(const F<PROJECT_NAME>CharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart, ECharacterPartSource::Natural);
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::AddCharacterPartInternal(const F<PROJECT_NAME>CharacterPart& NewPart, ECharacterPartSource Source)
{
	F<PROJECT_NAME>ControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;
	NewEntry.Source = Source;

	if (U<PROJECT_NAME>PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		if (NewEntry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
		{
			NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		}
	}

}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::RemoveCharacterPart(const F<PROJECT_NAME>CharacterPart& PartToRemove)
{
	for (auto EntryIt = CharacterParts.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (F<PROJECT_NAME>CharacterPart::AreEquivalentParts(EntryIt->Part, PartToRemove))
		{
			if (U<PROJECT_NAME>PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
			{
				PawnCustomizer->RemoveCharacterPart(EntryIt->Handle);
			}

			EntryIt.RemoveCurrent();
			break;
		}
	}
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (U<PROJECT_NAME>PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (F<PROJECT_NAME>ControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}

	CharacterParts.Reset();
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Remove from the old pawn
	if (U<PROJECT_NAME>PawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<U<PROJECT_NAME>PawnComponent_CharacterParts>() : nullptr)
	{
		for (F<PROJECT_NAME>ControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// Apply to the new pawn
	if (U<PROJECT_NAME>PawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<U<PROJECT_NAME>PawnComponent_CharacterParts>() : nullptr)
	{
		for (F<PROJECT_NAME>ControllerCharacterPartEntry& Entry : CharacterParts)
		{
			ensure(!Entry.Handle.IsValid());

			if (Entry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
			{
				Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
			}
		}
	}
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::ApplyDeveloperSettings()
{
#if UE_WITH_CHEAT_MANAGER
	const U<PROJECT_NAME>CosmeticDeveloperSettings* Settings = GetDefault<U<PROJECT_NAME>CosmeticDeveloperSettings>();

	// Suppress or unsuppress natural parts if needed
	const bool bSuppressNaturalParts = (Settings->CheatMode == ECosmeticCheatMode::ReplaceParts) && (Settings->CheatCosmeticCharacterParts.Num() > 0);
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);

	// Remove anything added by developer settings and re-add it
	U<PROJECT_NAME>PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaDeveloperSettingsCheat)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	// Add new parts
	for (const F<PROJECT_NAME>CharacterPart& PartDesc : Settings->CheatCosmeticCharacterParts)
	{
		AddCharacterPartInternal(PartDesc, ECharacterPartSource::AppliedViaDeveloperSettingsCheat);
	}
#endif
}


void U<PROJECT_NAME>ControllerComponent_CharacterParts::AddCheatPart(const F<PROJECT_NAME>CharacterPart& NewPart, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	AddCharacterPartInternal(NewPart, ECharacterPartSource::AppliedViaCheatManager);
#endif
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::ClearCheatParts()
{
#if UE_WITH_CHEAT_MANAGER
	U<PROJECT_NAME>PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	// Remove anything added by cheat manager cheats
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaCheatManager)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	ApplyDeveloperSettings();
#endif
}

void U<PROJECT_NAME>ControllerComponent_CharacterParts::SetSuppressionOnNaturalParts(bool bSuppressed)
{
#if UE_WITH_CHEAT_MANAGER
	U<PROJECT_NAME>PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	for (F<PROJECT_NAME>ControllerCharacterPartEntry& Entry : CharacterParts)
	{
		if ((Entry.Source == ECharacterPartSource::Natural) && bSuppressed)
		{
			// Suppress
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(Entry.Handle);
				Entry.Handle.Reset();
			}
			Entry.Source = ECharacterPartSource::NaturalSuppressedViaCheat;
		}
		else if ((Entry.Source == ECharacterPartSource::NaturalSuppressedViaCheat) && !bSuppressed)
		{
			// Unsuppress
			if (PawnCustomizer != nullptr)
			{
				Entry.Handle = PawnCustomizer->AddCharacterPart(Entry.Part);
			}
			Entry.Source = ECharacterPartSource::Natural;
		}
	}
#endif
}
