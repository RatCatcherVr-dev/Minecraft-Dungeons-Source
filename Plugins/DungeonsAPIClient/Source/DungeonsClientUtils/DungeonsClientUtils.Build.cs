namespace UnrealBuildTool.Rules
{
	public class DungeonsClientUtils : ModuleRules
	{
		public DungeonsClientUtils(ReadOnlyTargetRules Target) : base(Target)
		{
            PrivateDependencyModuleNames.AddRange(
                new[]
                {
                    "Engine",
                }
            );

            PublicDependencyModuleNames.AddRange(
				new []
				{
					"Core"
				}
			);
				
			PrivatePCHHeaderFile = "Private/DungeonsClientUtilsPrivatePCH.h";
        }
	}
}
