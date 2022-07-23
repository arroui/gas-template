// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class <PROJECT_NAME>EditorTarget : TargetRules
{
	public <PROJECT_NAME>EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange(new string[] { "<PROJECT_NAME>Game", "<PROJECT_NAME>Editor" });

		<PROJECT_NAME>GameTarget.ApplyShared<PROJECT_NAME>TargetSettings(this);

		// This is used for touch screen development along with the "Unreal Remote 2" app
		EnablePlugins.Add("RemoteSession");
	}
}
