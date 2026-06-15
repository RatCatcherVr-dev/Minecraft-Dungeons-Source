using System.IO;
using System;
using UnrealBuildTool;

public class minecraftapiclient : ModuleRules
{
	public minecraftapiclient(ReadOnlyTargetRules Target) : base(Target)
	{
        Type = ModuleType.External;
        bEnableExceptions = true;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        string BaseDir = ModuleDirectory;
        string LibrariesDir = BaseDir + "/../../../../../Source/SourceDepsModule/Libraries/";
        string SourceDir = BaseDir + "/../../../../../SourceDeps/";

        PublicIncludePaths.Add(SourceDir);
        PublicIncludePaths.Add(SourceDir + "json");
        PublicIncludePaths.Add(SourceDir + "Zlib");
        PublicDependencyModuleNames.Add("Core");
        PublicDependencyModuleNames.Add("CoreUObject");
        PublicDependencyModuleNames.Add("Engine");

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                    "DungeonsAPIClient",
                    "DungeonsClientUtils"
            }
        );

        // TODO: Fix up includes and libraries for openssl on all platforms

        switch (Target.Platform)
        {
            case UnrealTargetPlatform.Win64:
                AddJsonLibraryPaths("json", "x64", ".lib", LibrariesDir);
                AddJsonLibraryPaths("Zlib", "x64", ".lib", LibrariesDir);

                /*-------------- openssl -------------- */
                PublicIncludePaths.Add(EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1/Include/Win64/VS2015");
                PublicLibraryPaths.Add(EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1/Lib/Win64/VS2015/Release");
                PublicAdditionalLibraries.Add("libcrypto.lib");
                PublicAdditionalLibraries.Add("Crypt32.lib");
                /*------------------------------------ */
                break;
            case UnrealTargetPlatform.Switch:
                AddJsonLibraryPaths("json", "NX64", ".a", LibrariesDir);
                AddJsonLibraryPaths("Zlib", "NX64", ".a", LibrariesDir);
                /*-------------- openssl -------------- */
                PublicIncludePaths.Add(SourceDir + "external/switch");
                /*------------------------------------ */
                break;
            case UnrealTargetPlatform.PS4:
                AddJsonLibraryPaths("json", "PS4", ".a", LibrariesDir);
                AddJsonLibraryPaths("Zlib", "PS4", ".a", LibrariesDir);

                /*-------------- openssl -------------- */
                string IncludePath = EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1" + "/" + "include/PS4";
                string LibraryPath = EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1" + "/" + "lib/PS4/release";
                PublicIncludePaths.Add(IncludePath);
                PublicAdditionalLibraries.Add(LibraryPath + "/" + "OpenSSLOrbis.a");
                PublicAdditionalLibraries.Add(LibraryPath + "/" + "LibCryptoOrbis.a");
                /*------------------------------------ */
                break;
            case UnrealTargetPlatform.XboxOne:
                AddJsonLibraryPaths("json", "Durango", ".lib", LibrariesDir);
                AddJsonLibraryPaths("Zlib", "Durango", ".lib", LibrariesDir);

                /*-------------- openssl -------------- */
                PublicIncludePaths.Add(EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1/Include/Win64/VS2015");
                PublicLibraryPaths.Add(EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1/Lib/Win64/VS2015/Release");
                PublicAdditionalLibraries.Add(LibrariesDir + "/external/XboxOne/libcrypto.lib");
                PublicAdditionalLibraries.Add(LibrariesDir + "/external/XboxOne/libssl.lib");
                /*------------------------------------ */
                break;
            default:
                throw new BuildException("Unsupported target platform for SourceDeps.");
        }

        PublicIncludePaths.Add(SourceDir + "Themida");

        if (Target.Platform == UnrealTargetPlatform.Win64 && Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicAdditionalLibraries.Add(LibrariesDir + "Themida/SecureEngineSDK64.lib");
            PublicDefinitions.Add("USING_ANTICHEAT=1");
        }
    }

    private void AddJsonLibraryPaths(string Library, string PlatformDir, string LibFormat, string LibrariesDir)
    {
        PublicLibraryPaths.Add(LibrariesDir + Library + "/" + PlatformDir);
        PublicAdditionalLibraries.Add(LibrariesDir + Library + "/" + PlatformDir + "/" + Library + LibFormat);
    }
}

