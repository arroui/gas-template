// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>CheatManager.h"
#include "<PROJECT_NAME>PlayerController.h"
#include "<PROJECT_NAME>DebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/<PROJECT_NAME>AssetManager.h"
#include "System/<PROJECT_NAME>GameData.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "Character/<PROJECT_NAME>HealthComponent.h"
#include "Character/<PROJECT_NAME>PawnExtensionComponent.h"
#include "System/<PROJECT_NAME>SystemStatics.h"
#include "Development/<PROJECT_NAME>DeveloperSettings.h"

DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>Cheat);

namespace <PROJECT_NAME>Cheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("<PROJECT_NAME>Cheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("<PROJECT_NAME>Cheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


U<PROJECT_NAME>CheatManager::U<PROJECT_NAME>CheatManager()
{
	DebugCameraControllerClass = A<PROJECT_NAME>DebugCameraController::StaticClass();
}

void U<PROJECT_NAME>CheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const F<PROJECT_NAME>CheatToRun& CheatRow : GetDefault<U<PROJECT_NAME>DeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (<PROJECT_NAME>Cheat::bStartInGodMode)
	{
		God();	
	}
}

void U<PROJECT_NAME>CheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(Log<PROJECT_NAME>Cheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void U<PROJECT_NAME>CheatManager::Cheat(const FString& Msg)
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		<PROJECT_NAME>PC->ServerCheat(Msg.Left(128));
	}
}

void U<PROJECT_NAME>CheatManager::CheatAll(const FString& Msg)
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		<PROJECT_NAME>PC->ServerCheatAll(Msg.Left(128));
	}
}

void U<PROJECT_NAME>CheatManager::PlayNextGame()
{
	U<PROJECT_NAME>SystemStatics::PlayNextGame(this);
}

void U<PROJECT_NAME>CheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void U<PROJECT_NAME>CheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == <PROJECT_NAME>Cheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool U<PROJECT_NAME>CheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void U<PROJECT_NAME>CheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(<PROJECT_NAME>Cheat::NAME_Fixed);
	}
}

void U<PROJECT_NAME>CheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool U<PROJECT_NAME>CheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == <PROJECT_NAME>Cheat::NAME_Fixed);
	}

	return false;
}

void U<PROJECT_NAME>CheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void U<PROJECT_NAME>CheatManager::CycleDebugCameras()
{
	if (!<PROJECT_NAME>Cheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void U<PROJECT_NAME>CheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void U<PROJECT_NAME>CheatManager::CancelActivatedAbilities()
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		<PROJECT_NAME>ASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void U<PROJECT_NAME>CheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = F<PROJECT_NAME>GameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = GetPlayerAbilitySystemComponent())
		{
			<PROJECT_NAME>ASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Cheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void U<PROJECT_NAME>CheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = F<PROJECT_NAME>GameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = GetPlayerAbilitySystemComponent())
		{
			<PROJECT_NAME>ASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Cheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void U<PROJECT_NAME>CheatManager::DamageSelf(float DamageAmount)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(<PROJECT_NAME>ASC, DamageAmount);
	}
}

void U<PROJECT_NAME>CheatManager::DamageTarget(float DamageAmount)
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		if (<PROJECT_NAME>PC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			<PROJECT_NAME>PC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(<PROJECT_NAME>PC, TargetHitResult);

		if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>TargetASC = Cast<U<PROJECT_NAME>AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(<PROJECT_NAME>TargetASC, DamageAmount);
		}
	}
}

void U<PROJECT_NAME>CheatManager::ApplySetByCallerDamage(U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC, float DamageAmount)
{
	check(<PROJECT_NAME>ASC);

	TSubclassOf<UGameplayEffect> DamageGE = U<PROJECT_NAME>AssetManager::GetSubclass(U<PROJECT_NAME>GameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = <PROJECT_NAME>ASC->MakeOutgoingSpec(DamageGE, 1.0f, <PROJECT_NAME>ASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(F<PROJECT_NAME>GameplayTags::Get().SetByCaller_Damage, DamageAmount);
		<PROJECT_NAME>ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void U<PROJECT_NAME>CheatManager::HealSelf(float HealAmount)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(<PROJECT_NAME>ASC, HealAmount);
	}
}

void U<PROJECT_NAME>CheatManager::HealTarget(float HealAmount)
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(<PROJECT_NAME>PC, TargetHitResult);

		if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>TargetASC = Cast<U<PROJECT_NAME>AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(<PROJECT_NAME>TargetASC, HealAmount);
		}
	}
}

void U<PROJECT_NAME>CheatManager::ApplySetByCallerHeal(U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC, float HealAmount)
{
	check(<PROJECT_NAME>ASC);

	TSubclassOf<UGameplayEffect> HealGE = U<PROJECT_NAME>AssetManager::GetSubclass(U<PROJECT_NAME>GameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = <PROJECT_NAME>ASC->MakeOutgoingSpec(HealGE, 1.0f, <PROJECT_NAME>ASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(F<PROJECT_NAME>GameplayTags::Get().SetByCaller_Heal, HealAmount);
		<PROJECT_NAME>ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

U<PROJECT_NAME>AbilitySystemComponent* U<PROJECT_NAME>CheatManager::GetPlayerAbilitySystemComponent() const
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		return <PROJECT_NAME>PC->Get<PROJECT_NAME>AbilitySystemComponent();
	}
	return nullptr;
}

void U<PROJECT_NAME>CheatManager::DamageSelfDestruct()
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		if (const U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(<PROJECT_NAME>PC->GetPawn()))
		{
			if (PawnExtComp->IsPawnReadyToInitialize())
			{
				if (U<PROJECT_NAME>HealthComponent* HealthComponent = U<PROJECT_NAME>HealthComponent::FindHealthComponent(<PROJECT_NAME>PC->GetPawn()))
				{
					HealthComponent->DamageSelfDestruct();
				}
			}
		}
	}
}

void U<PROJECT_NAME>CheatManager::God()
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = Cast<A<PROJECT_NAME>PlayerController>(GetOuterAPlayerController()))
	{
		if (<PROJECT_NAME>PC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			<PROJECT_NAME>PC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = <PROJECT_NAME>PC->Get<PROJECT_NAME>AbilitySystemComponent())
		{
			const FGameplayTag Tag = F<PROJECT_NAME>GameplayTags::Get().Cheat_GodMode;
			const bool bHasTag = <PROJECT_NAME>ASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				<PROJECT_NAME>ASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				<PROJECT_NAME>ASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void U<PROJECT_NAME>CheatManager::UnlimitedHealth(int32 Enabled)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = F<PROJECT_NAME>GameplayTags::Get().Cheat_UnlimitedHealth;
		const bool bHasTag = <PROJECT_NAME>ASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				<PROJECT_NAME>ASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				<PROJECT_NAME>ASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}
