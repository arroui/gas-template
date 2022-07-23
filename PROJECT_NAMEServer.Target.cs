// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class <PROJECT_NAME>ServerTarget : TargetRules
{
	public <PROJECT_NAME>ServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange(new string[] { "<PROJECT_NAME>Game" });

		<PROJECT_NAME>GameTarget.ApplyShared<PROJECT_NAME>TargetSettings(this);

		bUseChecksInShipping = true;
	}
}
