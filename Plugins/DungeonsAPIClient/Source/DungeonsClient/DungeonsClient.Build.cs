namespace UnrealBuildTool.Rules
{
	public class DungeonsClient : ModuleRules
	{
		public DungeonsClient(ReadOnlyTargetRules Target) : base(Target)
		{
            bEnableExceptions = true;

            PrivateDependencyModuleNames.AddRange(
                new[]
                {
					"DungeonsHTTP",
                    "DungeonsClientUtils"
                }
            );
            
            PublicDependencyModuleNames.AddRange(
                new[] {
                    "Core",
                    "minecraftapiclient"
                }
            );

            PrivatePCHHeaderFile = "Private/DungeonsClientPrivatePCH.h";

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
