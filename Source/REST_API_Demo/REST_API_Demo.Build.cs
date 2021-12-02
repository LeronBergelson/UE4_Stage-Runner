// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class REST_API_Demo : ModuleRules
{
	public REST_API_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
