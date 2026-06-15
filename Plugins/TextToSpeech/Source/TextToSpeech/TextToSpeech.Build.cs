// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
namespace UnrealBuildTool.Rules
{
	public class TextToSpeech : ModuleRules
	{
        public TextToSpeech(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core" // from default
				}
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "CoreUObject",
                    "Engine",
                    "AudioMixer"
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {

                }
                );

            string BaseDir = ModuleDirectory;
            string LibrariesDir = BaseDir + "/../../../../Source/SourceDepsModule/Libraries/";
            string SourceDir = BaseDir + "/../../../../SourceDeps/";

            //D11.JPhoenix Add Editor dependencies if target type Editor
            bool bIsEditor = false;
            if (Target.Type == TargetType.Editor)
            {
                bIsEditor = true;
                PublicDefinitions.Add(string.Format("TTS_WITH_EDITOR={0}", (bIsEditor ? "1" : "0")));
            }

            bool UseFlite = false;

            if (Target.Platform == UnrealTargetPlatform.Win64 ||
                Target.Platform == UnrealTargetPlatform.Win32)
            {
                if (UseFlite)
                {
                    PublicLibraryPaths.Add(LibrariesDir + "Flite/x86");
                    PublicAdditionalLibraries.Add(LibrariesDir + "Flite/x86/Flite.lib");
                }
                else
                {
                    PrivateIncludePaths.Add("TextToSpeech/Private/Windows");
                    PrivateIncludePaths.Add(SourceDir + "atlmfc/include");
                    if (Target.Platform == UnrealTargetPlatform.Win32)
                    {
                        PublicAdditionalLibraries.Add(LibrariesDir + "atlmfc/x86/atls.lib");
                    }
                    else
                    {
                        PublicAdditionalLibraries.Add(LibrariesDir + "atlmfc/x64/atls.lib");
                    }
                }
            }
            else if (Target.Platform == UnrealTargetPlatform.XboxOne)
            {
                PrivateIncludePaths.Add("TextToSpeech/Private/XboxOne");
            }
            else if (Target.Platform == UnrealTargetPlatform.PS4)
            {
                UseFlite = true;
                PublicLibraryPaths.Add(LibrariesDir + "Flite/PS4");
                PublicAdditionalLibraries.Add(LibrariesDir + "Flite/PS4/Flite.a");
            }
            else if (Target.Platform == UnrealTargetPlatform.Switch)
            {
                UseFlite = true;
                PublicLibraryPaths.Add(LibrariesDir + "Flite/NX64");
                PublicAdditionalLibraries.Add(LibrariesDir + "Flite/NX64/Flite.a");
            }


            if (UseFlite)
            {
                PrivateIncludePaths.Add("TextToSpeech/Private/Flite");
                PublicDefinitions.Add("USE_FLITE_TTS");
                PublicDefinitions.Add("CST_NO_SOCKETS");
                PublicDefinitions.Add("CST_AUDIO_NONE");
                PublicDefinitions.Add("CST_MMAP_FILEIO_NONE");
                PublicDefinitions.Add("__palmos__=0");
                PrivateIncludePaths.Add(SourceDir + "flite/include");
            }
        }
    }
}
