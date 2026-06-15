namespace UnrealBuildTool.Rules
{
	public class DungeonsAuth : ModuleRules
	{
		public DungeonsAuth(ReadOnlyTargetRules Target) : base(Target)
		{
            bEnableExceptions = true;
            
            PrivateDependencyModuleNames.AddRange(
                new[]
                {
	                "DungeonsClientUtils",
	                "DungeonsClient"
                }
            );
            
            PublicDependencyModuleNames.AddRange(
                new[] {
                    "Core",
                    "DungeonsHTTP",
                    "minecraftapiclient"
                }
            );
			
			PrivatePCHHeaderFile = "Private/DungeonsAuthPrivatePCH.h";

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
