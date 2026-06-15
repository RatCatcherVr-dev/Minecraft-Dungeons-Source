// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonsEditorTarget : TargetRules
{
	public DungeonsEditorTarget( TargetInfo Target ) : base(Target)
	{
		Type = TargetType.Editor;
		//bForceEnableExceptions = true;

        ExtraModuleNames.Add("Dungeons");
        ExtraModuleNames.Add("DungeonsEditor");
        ExtraModuleNames.Add("TileDecorator");
    }
}
