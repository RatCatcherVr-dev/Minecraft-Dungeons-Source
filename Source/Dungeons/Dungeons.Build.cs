// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System.IO;

using UnrealBuildTool;

public class Dungeons : ModuleRules
{
    public Dungeons(ReadOnlyTargetRules Target) : base(Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
			"Slate",
            "RenderCore",
            "RHI",
            "RuntimeMeshComponent",
            "ImageWrapper",
            "LevelSequence",
            "MovieScene",
			"MoviePlayer",
            "CinematicCamera",
            "UMG",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "GameplayTags",
            "GameplayAbilities",
            "HTTP",
            "AssetRegistry",
            "TextToSpeech",
            "WebBrowser",
            "JSON"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
             "SourceDepsModule",
             "SlateCore",
             "TdkRpc",
             "ReplicationGraph",
         });
        
        PublicDependencyModuleNames.Add("DungeonsAPIClient");
        PublicDependencyModuleNames.Add("DungeonsTrials");
        PublicDependencyModuleNames.Add("DungeonsEntitlements");
        PublicDependencyModuleNames.Add("DungeonsAuth");
        PublicDependencyModuleNames.Add("DungeonsClient");
        PublicDependencyModuleNames.Add("DungeonsLiveOps");

        string SourceDepsModuleDir = ModuleDirectory + "/../" + "SourceDepsModule";
        if (!Directory.Exists(SourceDepsModuleDir))
        {
            string Err = string.Format("SourceDepsModule not found in {0}", SourceDepsModuleDir);
            System.Console.WriteLine(Err);
            throw new BuildException(Err);
        }

        bool DungeonOssEnabled = true;

        PrivatePCHHeaderFile = "Dungeons.h";

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("ApplicationCore");
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
			PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
            PublicDefinitions.Add("OS_WIN");
            PublicDefinitions.Add("MCPE_PLATFORM_WIN32");
            PublicDefinitions.Add("MCPE_PLATFORM_WINDOWS_FAMILY");
            PrivateDependencyModuleNames.Add("UnrealToolsFramework");

#if UE4_WITH_CUSTOM_WIN10_SUPPORT
            if (Target.WindowsPlatform.bTargetWin10)
            {
                DynamicallyLoadedModuleNames.Add("GDK");
            }
#endif
#if UE4_WITH_CUSTOM_EPIC_STORE_SUPPORT
            if (Target.WindowsPlatform.bTargetEpicStore)
            {
                PrivateDependencyModuleNames.Add("EpicServices");
                PrivateDefinitions.Add("EPIC_STORE_BUILD=1");
            }
#endif
#if UE4_WITH_CUSTOM_STEAM_SUPPORT
        if (Target.WindowsPlatform.bTargetSteam)
        {
            PrivateDefinitions.Add("STEAM_BUILD=1");
            DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
            PrivateDependencyModuleNames.Add("Steamworks");
        }
#endif

        }
        else if(Target.Platform == UnrealTargetPlatform.Win32)
        {
            PrivateDependencyModuleNames.Add("UnrealToolsFramework");
            DynamicallyLoadedModuleNames.Add("WebBrowser");
        }
        else if(Target.Platform == UnrealTargetPlatform.XboxOne)
        {
            PublicAdditionalLibraries.Add("xgamestreaming.lib");
            PrivateDependencyModuleNames.Add("UnrealToolsFramework");
        }
        else if (Target.Platform == UnrealTargetPlatform.PS4)
        {
            DungeonOssEnabled = true;
            DynamicallyLoadedModuleNames.Add("OnlineSubsystemPS4");
            PublicDefinitions.Add("ACHIEVEMENTS_ENABLED=1");
            PrivateDependencyModuleNames.Add("PlayfabServices");
        }
        else if (Target.Platform == UnrealTargetPlatform.Switch)
        {
            //D11.PS - to disable the dungeons OSS on switch uncomment out the following.
            //DungeonOssEnabled = false;
            //DynamicallyLoadedModuleNames.Add("OnlineSubsystemSwitch");
        }

        if (Target.Configuration != UnrealTargetConfiguration.Shipping && (
            Target.Platform == UnrealTargetPlatform.XboxOne ||
            Target.Platform == UnrealTargetPlatform.Win32 ||
            Target.Platform == UnrealTargetPlatform.Win64))
        {
            PrivateDependencyModuleNames.Add("BotAutomation");
        }

        if (DungeonOssEnabled)
        {
            DynamicallyLoadedModuleNames.Add("OnlineSubsystemDungeons");
            PublicDefinitions.Add("ACHIEVEMENTS_ENABLED=1");
            PublicDefinitions.Add("DUNGEONS_OSS_ENABLED=1");
            PrivateDependencyModuleNames.Add("PlayfabServices");
        }
        else if (Target.Platform != UnrealTargetPlatform.PS4)
        {
            PublicDefinitions.Add("ACHIEVEMENTS_ENABLED=0");
            PublicDefinitions.Add("DUNGEONS_OSS_ENABLED=0");
        }
        else
        {
            PublicDefinitions.Add("DUNGEONS_OSS_ENABLED=0");
        }
        bEnableExceptions = true;

        if (Target.Configuration == UnrealTargetConfiguration.Development)
        {
            bFasterWithoutUnity = false;
        }
        else if (Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("PUBLISH=1");
        }

        if (Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("PUBLISH=1");
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicDefinitions.Add("USING_ANTICHEAT=1");
            }
        }
    }
}
