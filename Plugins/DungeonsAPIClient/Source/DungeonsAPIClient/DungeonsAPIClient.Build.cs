namespace UnrealBuildTool.Rules
{
	public class DungeonsAPIClient : ModuleRules
	{
		public DungeonsAPIClient(ReadOnlyTargetRules Target) : base(Target)
		{
			bEnableExceptions = true;

			PublicDependencyModuleNames.AddRange(
				new[]
				{
					"Core",
					"DungeonsAuth",
					"DungeonsTrials",
					"DungeonsEntitlements",
					"DungeonsClient",
                    "DungeonsLiveOps"
				}
			);

			PrivatePCHHeaderFile = "Private/DungeonsAPIClientPrivatePCH.h";

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
