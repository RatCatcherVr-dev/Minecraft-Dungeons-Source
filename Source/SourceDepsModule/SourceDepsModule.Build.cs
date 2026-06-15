using UnrealBuildTool;

public class SourceDepsModule : ModuleRules
{
	public SourceDepsModule(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

        string BaseDir = ModuleDirectory;
		string LibrariesDir = BaseDir + "/Libraries/";
		string SourceDir = BaseDir + "/../../SourceDeps/";
		string PlatformDir = "";
		string LibFormat = "";

        switch (Target.Platform)
        {
            case UnrealTargetPlatform.Win64:
                PlatformDir = "x64";
                LibFormat = ".lib";
                break;
            case UnrealTargetPlatform.Switch:
                PlatformDir = "NX64";
                LibFormat = ".a";
                break;
            case UnrealTargetPlatform.PS4:
                PlatformDir = "PS4";
                LibFormat = ".a";
                break;
            case UnrealTargetPlatform.XboxOne:
                PlatformDir = "Durango";
                LibFormat = ".lib";
                break;
            default:
                throw new BuildException("Unsupported target platform for SourceDeps.");
        }

        PublicIncludePaths.Add(SourceDir);
        PublicIncludePaths.Add(SourceDir + "json");
        PublicIncludePaths.Add(SourceDir + "zlib");
        PublicLibraryPaths.Add(LibrariesDir + "Zlib/" + PlatformDir);
        PublicLibraryPaths.Add(LibrariesDir + "json/" + PlatformDir);
        PublicAdditionalLibraries.Add(LibrariesDir + "Zlib/" + PlatformDir + "/Zlib" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "json/" + PlatformDir + "/json" + LibFormat);

        PublicIncludePaths.Add(SourceDir + "Themida");

        if (Target.Platform == UnrealTargetPlatform.Win64 && Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicAdditionalLibraries.Add(LibrariesDir + "Themida/SecureEngineSDK64.lib");
        }
	}
}