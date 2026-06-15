// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class DungeonsServerTarget : TargetRules
{
       public DungeonsServerTarget(TargetInfo Target) : base(Target)
       {
        Type = TargetType.Server;
        
        bForceEnableExceptions = true;
		
        ExtraModuleNames.Add("Dungeons"); 
       }
}