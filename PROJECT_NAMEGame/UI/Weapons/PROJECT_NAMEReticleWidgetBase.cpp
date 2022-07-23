// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ReticleWidgetBase.h"
#include "Weapons/<PROJECT_NAME>WeaponInstance.h"
#include "Weapons/<PROJECT_NAME>RangedWeaponInstance.h"
#include "Inventory/<PROJECT_NAME>InventoryItemInstance.h"

U<PROJECT_NAME>ReticleWidgetBase::U<PROJECT_NAME>ReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>ReticleWidgetBase::InitializeFromWeapon(U<PROJECT_NAME>WeaponInstance* InWeapon)
{
	WeaponInstance = InWeapon;
	InventoryInstance = nullptr;
	if (WeaponInstance)
	{
		InventoryInstance = Cast<U<PROJECT_NAME>InventoryItemInstance>(WeaponInstance->GetInstigator());
	}
	OnWeaponInitialized();
}


float U<PROJECT_NAME>ReticleWidgetBase::ComputeSpreadAngle() const
{
	if (const U<PROJECT_NAME>RangedWeaponInstance* RangedWeapon = Cast<const U<PROJECT_NAME>RangedWeaponInstance>(WeaponInstance))
	{
		const float BaseSpreadAngle = RangedWeapon->GetCalculatedSpreadAngle();
		const float SpreadAngleMultiplier = RangedWeapon->GetCalculatedSpreadAngleMultiplier();
		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

		return ActualSpreadAngle;
	}
	else
	{
		return 0.0f;
	}
}

bool U<PROJECT_NAME>ReticleWidgetBase::HasFirstShotAccuracy() const
{
	if (const U<PROJECT_NAME>RangedWeaponInstance* RangedWeapon = Cast<const U<PROJECT_NAME>RangedWeaponInstance>(WeaponInstance))
	{
		return RangedWeapon->HasFirstShotAccuracy();
	}
	else
	{
		return false;
	}
}

float U<PROJECT_NAME>ReticleWidgetBase::ComputeMaxScreenspaceSpreadRadius() const
{
	const float LongShotDistance = 10000.f;

	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->PlayerCameraManager)
	{
		// A weapon's spread can be thought of as a cone shape. To find the screenspace spread for reticle visualization,
		// we create a line on the edge of the cone at a long distance. The end of that point is on the edge of the cone's circle.
		// We then project it back onto the screen. Its distance from screen center is the spread radius.

		// This isn't perfect, due to there being some distance between the camera location and the gun muzzle.

		const float SpreadRadiusRads = FMath::DegreesToRadians(ComputeSpreadAngle() * 0.5f);
		const float SpreadRadiusAtDistance = FMath::Tan(SpreadRadiusRads) * LongShotDistance;

		FVector CamPos;
		FRotator CamOrient;
		PC->PlayerCameraManager->GetCameraViewPoint(CamPos, CamOrient);

		FVector CamForwDir = CamOrient.RotateVector(FVector::ForwardVector);
		FVector CamUpDir   = CamOrient.RotateVector(FVector::UpVector);

		FVector OffsetTargetAtDistance = CamPos + (CamForwDir * LongShotDistance) + (CamUpDir * SpreadRadiusAtDistance);

		FVector2D OffsetTargetInScreenspace;

		if (PC->ProjectWorldLocationToScreen(OffsetTargetAtDistance, OffsetTargetInScreenspace, true))
		{
			int32 ViewportSizeX(0), ViewportSizeY(0);
			PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

			const FVector2D ScreenSpaceCenter(FVector::FReal(ViewportSizeX) * 0.5f, FVector::FReal(ViewportSizeY) * 0.5f);

			return (OffsetTargetInScreenspace - ScreenSpaceCenter).Length();
		}
	}
	
	return 0.0f;
}