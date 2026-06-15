using UnrealBuildTool;
using System.IO;

public class EpicServices : ModuleRules
{
	public EpicServices(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.Add("Core");			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "EngineSettings"
            }
		);        

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibraryDir = "$(ProjectDir)/Plugins/EpicServices/Libraries/eos_sdk";
            PublicAdditionalLibraries.Add(Path.Combine(LibraryDir, "Lib/EOSSDK-Win64-Shipping.lib"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/EOSSDK-Win64-Shipping.dll", Path.Combine(LibraryDir, "Bin/EOSSDK-Win64-Shipping.dll"));
            PublicIncludePaths.Add(Path.Combine(LibraryDir, "Include"));

            PublicIncludePaths.Add("$(ProjectDir)/SourceDeps/Themida");
            if (Target.Configuration == UnrealTargetConfiguration.Shipping)
            {
                PublicAdditionalLibraries.Add("$(ProjectDir)/Source/SourceDepsModule/Libraries/Themida/SecureEngineSDK64.lib");
                PublicDefinitions.Add("USING_ANTICHEAT=1");
            }
        }
        if (Target.Platform != UnrealTargetPlatform.Win64)
            PrecompileForTargets = PrecompileTargetsType.None;        
    }
}
