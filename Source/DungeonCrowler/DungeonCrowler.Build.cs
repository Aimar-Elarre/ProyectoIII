// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DungeonCrowler : ModuleRules
{
	public DungeonCrowler(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Include paths para las carpetas organizadas
		PublicIncludePaths.AddRange(new string[]
		{
			System.IO.Path.Combine(ModuleDirectory),
			System.IO.Path.Combine(ModuleDirectory, "Core"),
			System.IO.Path.Combine(ModuleDirectory, "Player"),
			System.IO.Path.Combine(ModuleDirectory, "Inventory"),
			System.IO.Path.Combine(ModuleDirectory, "Enemies"),
			System.IO.Path.Combine(ModuleDirectory, "Traps"),
			System.IO.Path.Combine(ModuleDirectory, "Mechanics"),
			System.IO.Path.Combine(ModuleDirectory, "Events")
		});

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
        "Core",
        "CoreUObject",
        "Engine",
        "InputCore",
        "AIModule",
        "EnhancedInput",
        "Niagara"
            });
        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
