// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SilentRedServerTarget : TargetRules
{
	public SilentRedServerTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "SilentRedServer" } );
		bUsesSteam = true;
		GlobalDefinitions.Add("UE4_PROJECT_STEAMDESC=\"Steam Server Test\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMPRODUCTNAME=\"SilentRed\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMGAMEDIR=\"SilentRed\"");
		GlobalDefinitions.Add("UE4_PROJECT_STEAMSHIPPINGID=\"1145390\"");
	}
}
