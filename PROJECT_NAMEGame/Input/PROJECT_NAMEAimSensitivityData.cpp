// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AimSensitivityData.h"

U<PROJECT_NAME>AimSensitivityData::U<PROJECT_NAME>AimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ E<PROJECT_NAME>GamepadSensitivity::Slow,			0.5f },
		{ E<PROJECT_NAME>GamepadSensitivity::SlowPlus,		0.75f },
		{ E<PROJECT_NAME>GamepadSensitivity::SlowPlusPlus,	0.9f },
		{ E<PROJECT_NAME>GamepadSensitivity::Normal,		1.0f },
		{ E<PROJECT_NAME>GamepadSensitivity::NormalPlus,	1.1f },
		{ E<PROJECT_NAME>GamepadSensitivity::NormalPlusPlus,1.25f },
		{ E<PROJECT_NAME>GamepadSensitivity::Fast,			1.5f },
		{ E<PROJECT_NAME>GamepadSensitivity::FastPlus,		1.75f },
		{ E<PROJECT_NAME>GamepadSensitivity::FastPlusPlus,	2.0f },
		{ E<PROJECT_NAME>GamepadSensitivity::Insane,		2.5f },
	};
}

const float U<PROJECT_NAME>AimSensitivityData::SensitivtyEnumToFloat(const E<PROJECT_NAME>GamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}
