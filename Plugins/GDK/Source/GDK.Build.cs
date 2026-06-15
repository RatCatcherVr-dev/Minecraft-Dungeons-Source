using UnrealBuildTool;
using System.IO;
using System;
using System.Reflection;

public class GDK : ModuleRules
{
    public GDK(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Private/PCHGDK.h";

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                            "Core",
                            "Engine"
            });

#if UE4_WITH_CUSTOM_WIN10_SUPPORT
        if (Target.Platform == UnrealTargetPlatform.Win64
            && Target.WindowsPlatform.bTargetWin10 == true)
        {
            string BaseDir = ModuleDirectory;
            string LibrariesDir = BaseDir + "/../../../Source/SourceDepsModule/Libraries/";
            PublicLibraryPaths.Add(LibrariesDir + @"GDKThunks\Win64\Release");

            PublicAdditionalLibraries.Add("XGamingRuntimeThunks.lib");
            RuntimeDependencies.Add(@"$(BinaryOutputDir)\XGamingRuntimeThunks.dll", Path.Combine(LibrariesDir, @"GDKThunks\Win64\Release\XGamingRuntimeThunks.dll"));
        }
#endif
    }
}
 