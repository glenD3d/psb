// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PSB : ModuleRules
{
	public PSB(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MotionWarping"});

		PublicIncludePaths.Add("PSB/");

		PrivateDependencyModuleNames.AddRange(new string[] {
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});
	}
}
