// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>Character.h"
#include "<PROJECT_NAME>CharacterMovementComponent.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "Character/<PROJECT_NAME>PawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Camera/<PROJECT_NAME>CameraComponent.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "Character/<PROJECT_NAME>HealthComponent.h"
#include "Player/<PROJECT_NAME>PlayerController.h"
#include "Player/<PROJECT_NAME>PlayerState.h"
#include "System/<PROJECT_NAME>SignificanceManager.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

static FName NAME_<PROJECT_NAME>CharacterCollisionProfile_Capsule(TEXT("<PROJECT_NAME>PawnCapsule"));
static FName NAME_<PROJECT_NAME>CharacterCollisionProfile_Mesh(TEXT("<PROJECT_NAME>PawnMesh"));

A<PROJECT_NAME>Character::A<PROJECT_NAME>Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<U<PROJECT_NAME>CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_<PROJECT_NAME>CharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_<PROJECT_NAME>CharacterCollisionProfile_Mesh);

	U<PROJECT_NAME>CharacterMovementComponent* <PROJECT_NAME>MoveComp = CastChecked<U<PROJECT_NAME>CharacterMovementComponent>(GetCharacterMovement());
	<PROJECT_NAME>MoveComp->GravityScale = 1.0f;
	<PROJECT_NAME>MoveComp->MaxAcceleration = 2400.0f;
	<PROJECT_NAME>MoveComp->BrakingFrictionFactor = 1.0f;
	<PROJECT_NAME>MoveComp->BrakingFriction = 6.0f;
	<PROJECT_NAME>MoveComp->GroundFriction = 8.0f;
	<PROJECT_NAME>MoveComp->BrakingDecelerationWalking = 1400.0f;
	<PROJECT_NAME>MoveComp->bUseControllerDesiredRotation = false;
	<PROJECT_NAME>MoveComp->bOrientRotationToMovement = false;
	<PROJECT_NAME>MoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	<PROJECT_NAME>MoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	<PROJECT_NAME>MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	<PROJECT_NAME>MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	<PROJECT_NAME>MoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<U<PROJECT_NAME>PawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<U<PROJECT_NAME>HealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	CameraComponent = CreateDefaultSubobject<U<PROJECT_NAME>CameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void A<PROJECT_NAME>Character::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void A<PROJECT_NAME>Character::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (U<PROJECT_NAME>SignificanceManager* SignificanceManager = USignificanceManager::Get<U<PROJECT_NAME>SignificanceManager>(World))
		{
//@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
		}
	}
}

void A<PROJECT_NAME>Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (U<PROJECT_NAME>SignificanceManager* SignificanceManager = USignificanceManager::Get<U<PROJECT_NAME>SignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void A<PROJECT_NAME>Character::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void A<PROJECT_NAME>Character::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	DOREPLIFETIME(ThisClass, MyTeamID)
}

void A<PROJECT_NAME>Character::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void A<PROJECT_NAME>Character::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Update our team ID based on the controller
	if (HasAuthority() && (Controller != nullptr))
	{
		if (I<PROJECT_NAME>TeamAgentInterface* ControllerWithTeam = Cast<I<PROJECT_NAME>TeamAgentInterface>(Controller))
		{
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
		}
	}
}

A<PROJECT_NAME>PlayerController* A<PROJECT_NAME>Character::Get<PROJECT_NAME>PlayerController() const
{
	return CastChecked<A<PROJECT_NAME>PlayerController>(Controller, ECastCheckedType::NullAllowed);
}

A<PROJECT_NAME>PlayerState* A<PROJECT_NAME>Character::Get<PROJECT_NAME>PlayerState() const
{
	return CastChecked<A<PROJECT_NAME>PlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

U<PROJECT_NAME>AbilitySystemComponent* A<PROJECT_NAME>Character::Get<PROJECT_NAME>AbilitySystemComponent() const
{
	return Cast<U<PROJECT_NAME>AbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* A<PROJECT_NAME>Character::GetAbilitySystemComponent() const
{
	return PawnExtComponent->Get<PROJECT_NAME>AbilitySystemComponent();
}

void A<PROJECT_NAME>Character::OnAbilitySystemInitialized()
{
	U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent();
	check(<PROJECT_NAME>ASC);

	HealthComponent->InitializeWithAbilitySystem(<PROJECT_NAME>ASC);

	InitializeGameplayTags();
}

void A<PROJECT_NAME>Character::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void A<PROJECT_NAME>Character::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void A<PROJECT_NAME>Character::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (I<PROJECT_NAME>TeamAgentInterface* ControllerAsTeamProvider = Cast<I<PROJECT_NAME>TeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void A<PROJECT_NAME>Character::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void A<PROJECT_NAME>Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void A<PROJECT_NAME>Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void A<PROJECT_NAME>Character::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		const F<PROJECT_NAME>GameplayTags& GameplayTags = F<PROJECT_NAME>GameplayTags::Get();

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				<PROJECT_NAME>ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				<PROJECT_NAME>ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		U<PROJECT_NAME>CharacterMovementComponent* <PROJECT_NAME>MoveComp = CastChecked<U<PROJECT_NAME>CharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(<PROJECT_NAME>MoveComp->MovementMode, <PROJECT_NAME>MoveComp->CustomMovementMode, true);
	}
}

void A<PROJECT_NAME>Character::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		<PROJECT_NAME>ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool A<PROJECT_NAME>Character::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		return <PROJECT_NAME>ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool A<PROJECT_NAME>Character::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		return <PROJECT_NAME>ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool A<PROJECT_NAME>Character::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		return <PROJECT_NAME>ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void A<PROJECT_NAME>Character::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void A<PROJECT_NAME>Character::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void A<PROJECT_NAME>Character::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void A<PROJECT_NAME>Character::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	U<PROJECT_NAME>CharacterMovementComponent* <PROJECT_NAME>MoveComp = CastChecked<U<PROJECT_NAME>CharacterMovementComponent>(GetCharacterMovement());
	<PROJECT_NAME>MoveComp->StopMovementImmediately();
	<PROJECT_NAME>MoveComp->DisableMovement();
}

void A<PROJECT_NAME>Character::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void A<PROJECT_NAME>Character::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		if (<PROJECT_NAME>ASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void A<PROJECT_NAME>Character::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	U<PROJECT_NAME>CharacterMovementComponent* <PROJECT_NAME>MoveComp = CastChecked<U<PROJECT_NAME>CharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(<PROJECT_NAME>MoveComp->MovementMode, <PROJECT_NAME>MoveComp->CustomMovementMode, true);
}

void A<PROJECT_NAME>Character::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		const F<PROJECT_NAME>GameplayTags& GameplayTags = F<PROJECT_NAME>GameplayTags::Get();
		const FGameplayTag* MovementModeTag = nullptr;

		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = GameplayTags.CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = GameplayTags.MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			<PROJECT_NAME>ASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void A<PROJECT_NAME>Character::ToggleCrouch()
{
	const U<PROJECT_NAME>CharacterMovementComponent* <PROJECT_NAME>MoveComp = CastChecked<U<PROJECT_NAME>CharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || <PROJECT_NAME>MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (<PROJECT_NAME>MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void A<PROJECT_NAME>Character::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		<PROJECT_NAME>ASC->SetLooseGameplayTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void A<PROJECT_NAME>Character::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		<PROJECT_NAME>ASC->SetLooseGameplayTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool A<PROJECT_NAME>Character::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void A<PROJECT_NAME>Character::OnRep_ReplicatedAcceleration()
{
	if (U<PROJECT_NAME>CharacterMovementComponent* <PROJECT_NAME>MovementComponent = Cast<U<PROJECT_NAME>CharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel         = <PROJECT_NAME>MovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians   = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		<PROJECT_NAME>MovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void A<PROJECT_NAME>Character::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId A<PROJECT_NAME>Character::GetGenericTeamId() const
{
	return MyTeamID;
}

FOn<PROJECT_NAME>TeamIndexChangedDelegate* A<PROJECT_NAME>Character::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void A<PROJECT_NAME>Character::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void A<PROJECT_NAME>Character::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}
