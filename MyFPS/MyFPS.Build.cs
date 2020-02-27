// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyFPS : ModuleRules
{
    public MyFPS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "OnlineSubsystem", "OnlineSubsystemUtils" });
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
        }
    }
}
