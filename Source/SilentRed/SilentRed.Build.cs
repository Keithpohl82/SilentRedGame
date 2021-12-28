// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SilentRed : ModuleRules
{
	public SilentRed(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "Core", "CoreUObject", "Engine", "InputCore", "UMG", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks", "Sockets", "OnlineSubsystemEOS", "OnlineSubsystemEOSPlus"});

		PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils", "Sockets", "Steamworks", "OnlineSubsystemEOS", "OnlineSubsystemEOSPlus" });

		// Uncomment if you are using Slate UI
		 PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		 PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
