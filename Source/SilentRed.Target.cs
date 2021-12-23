// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SilentRedTarget : TargetRules
{
	public SilentRedTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "SilentRed" } );
		bUsesSteam = true;
		GlobalDefinitions.Add("UE4_PROJECT_STEAMDESC=\"Steam Server Test\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMPRODUCTNAME=\"SilentRed\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMGAMEDIR=\"SilentRed\"");
	}
}
