// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class MyFPSServerTarget : TargetRules   // Change this line as shown previously
{
    public MyFPSServerTarget(TargetInfo Target) : base(Target)  // Change this line as shown previously
    {
        Type = TargetType.Server;
        bUsesSteam = true;
        //bCompileSteamOSS = true;
        
        ExtraModuleNames.Add("MyFPS");    // Change this line as shown previously
    }
}