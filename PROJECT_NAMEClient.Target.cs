// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class <PROJECT_NAME>ClientTarget : TargetRules
{
	public <PROJECT_NAME>ClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange(new string[] { "<PROJECT_NAME>Game" });

		<PROJECT_NAME>GameTarget.ApplyShared<PROJECT_NAME>TargetSettings(this);
	}
}
