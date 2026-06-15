namespace UnrealBuildTool.Rules
{
	public class DungeonsTrials : ModuleRules
	{
		public DungeonsTrials(ReadOnlyTargetRules Target) : base(Target)
		{
            bEnableExceptions = true;
            
            PrivateDependencyModuleNames.AddRange(
                new[]
                {
                    "DungeonsClientUtils",
                    "DungeonsAuth",
                    "DungeonsClient"
                }
            );
            
            PublicDependencyModuleNames.AddRange(
				new []
				{
					"Core",
                    "minecraftapiclient"
				}
			);
				
			PrivatePCHHeaderFile = "Private/DungeonsTrialsPrivatePCH.h";

            string BaseDir = ModuleDirectory;
            string LibrariesDir = BaseDir + "/../../../../Source/SourceDepsModule/Libraries/";
            string SourceDir = BaseDir + "/../../../../SourceDeps/";

            PublicIncludePaths.Add(SourceDir + "Themida");

            if (Target.Platform == UnrealTargetPlatform.Win64 && Target.Configuration == UnrealTargetConfiguration.Shipping)
            {
                PublicAdditionalLibraries.Add(LibrariesDir + "Themida/SecureEngineSDK64.lib");
                PublicDefinitions.Add("USING_ANTICHEAT=1");
            }
        }
	}
}
