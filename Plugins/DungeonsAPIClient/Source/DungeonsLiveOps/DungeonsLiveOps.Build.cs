namespace UnrealBuildTool.Rules
{
	public class DungeonsLiveOps : ModuleRules
	{
		public DungeonsLiveOps(ReadOnlyTargetRules Target) : base(Target)
		{
            bEnableExceptions = true;
            
            PrivateDependencyModuleNames.AddRange(
                new[]
                {
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
				
			PrivatePCHHeaderFile = "Private/DungeonsLiveOpsPrivatePCH.h";

            string BaseDir = ModuleDirectory;
            string LibrariesDir = BaseDir + "/../../../../Source/SourceDepsModule/Libraries/";
            string SourceDir = BaseDir + "/../../../../SourceDeps/";
        }
	}
}
