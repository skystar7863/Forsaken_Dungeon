// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EchoOfMind : ModuleRules
{
    public EchoOfMind(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",
            "Niagara"
        });

        PublicIncludePaths.AddRange(new string[] { "EchoOfMind" });

        // 🔄 바꾼 부분: #if WITH_EDITOR → if (Target.bBuildEditor)
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "Blutility",
                "MainFrame",
                "EditorSubsystem",
                "Documentation"
            });
        }
    }
}
