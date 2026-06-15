// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonsTarget : TargetRules
{
	public DungeonsTarget( TargetInfo Target ) : base(Target)
	{
		Type = TargetType.Game;

		//D11.PS
        bForceEnableExceptions = true;
        if (Target.Platform == UnrealTargetPlatform.Win32 ||
            Target.Platform == UnrealTargetPlatform.Win64)
        {
            bForceEnableExceptions = false;

        }

		ExtraModuleNames.Add("Dungeons");
// 
//         //D11.PS - add Title data TODO - add correct data
// 		//enable this to allow online multiplayer
//         if (Target.Platform == UnrealTargetPlatform.PS4)
//         {
//             GlobalDefinitions.Add("UE4_PROJECT_NPTITLEID=NPXX53438_00");
//             GlobalDefinitions.Add("UE4_PROJECT_NPTITLESECRET=e903d77e885e278339a8f8c72733da598c36347a6df1e9b86ca44258315d99856ccafaabbc307771082f7b048815b6a656704a95be41eb380bd9d4ab05e10bba5882e673dc05538f3e55824a5ab2d5002fb6f43efe12f257de3efacf6cae749bdb581a7f24bcc00b0f9d80dca399c5f01befb24e5fd357779a9d2549fb85b776");
//         }


        //D11.SC Force Stats on in test
        if (Target.Configuration == UnrealTargetConfiguration.Test)
        {
            GlobalDefinitions.Add("FORCE_USE_STATS");
            bUseLoggingInShipping = true;
        }


    }
}
