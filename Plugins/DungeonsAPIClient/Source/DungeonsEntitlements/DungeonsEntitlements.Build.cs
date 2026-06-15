namespace UnrealBuildTool.Rules
{
	public class DungeonsEntitlements : ModuleRules
	{
		public DungeonsEntitlements(ReadOnlyTargetRules Target) : base(Target)
		{
            bEnableExceptions = true;
            
            PrivateDependencyModuleNames.AddRange(
                new[]
                {
                    "DungeonsClientUtils",
                    "DungeonsHTTP",
                    "DungeonsAuth"
                }
            );
            
            PublicDependencyModuleNames.AddRange(
                new[]
				{
                    "Core",
                    "minecraftapiclient"
				}
			);
				
			PrivatePCHHeaderFile = "Private/DungeonsEntitlementsPrivatePCH.h";

            string BaseDir = ModuleDirectory;
            string LibrariesDir = BaseDir + "/../../../../Source/SourceDepsModule/Libraries/";
            string SourceDir = BaseDir + "/../../../../SourceDeps/";

            PublicIncludePaths.Add(SourceDir + "Themida");

            if (Target.Platform == UnrealTargetPlatform.Win64 && Target.Configuration == UnrealTargetConfiguration.Shipping)
            {
                PublicAdditionalLibraries.Add(LibrariesDir + "Themida/SecureEngineSDK64.lib");
                PublicDefinitions.Add("USING_ANTICHEAT=1");
            }

#if UE4_WITH_CUSTOM_WIN10_SUPPORT
            if (Target.Platform == UnrealTargetPlatform.Win64 && Target.WindowsPlatform.bTargetWin10 == true)
            {
                PublicDependencyModuleNames.Add("GDK");
            }
#endif
        }
    }
}
