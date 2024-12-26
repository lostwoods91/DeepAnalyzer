// Copyright (C) 2024 Loris De Marchi. All rights reserved.

using UnrealBuildTool;

public class DeepAnalyzer : ModuleRules
{
    public DeepAnalyzer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
				// ... add other public dependencies that you statically link with here ...
			}
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Blutility",
                "CoreUObject",
                "DeveloperSettings",
                "EditorFramework",
                "EditorSubsystem",
                "Engine",
                "InputCore",
                "Projects",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "UMGEditor",
                "UnrealEd",
            }
            );
    }
}
