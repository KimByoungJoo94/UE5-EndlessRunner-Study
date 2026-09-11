// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EndlessRunner : ModuleRules
{
	public EndlessRunner(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"EndlessRunner",
			"EndlessRunner/Variant_Platforming",
			"EndlessRunner/Variant_Platforming/Animation",
			"EndlessRunner/Variant_Combat",
			"EndlessRunner/Variant_Combat/AI",
			"EndlessRunner/Variant_Combat/Animation",
			"EndlessRunner/Variant_Combat/Gameplay",
			"EndlessRunner/Variant_Combat/Interfaces",
			"EndlessRunner/Variant_Combat/UI",
			"EndlessRunner/Variant_SideScrolling",
			"EndlessRunner/Variant_SideScrolling/AI",
			"EndlessRunner/Variant_SideScrolling/Gameplay",
			"EndlessRunner/Variant_SideScrolling/Interfaces",
			"EndlessRunner/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
