using UnrealBuildTool;

public class PlayfabServices : ModuleRules
{
    public PlayfabServices(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
            }
            );
                
        
        PrivateIncludePaths.AddRange(
            new string[] 
            {
                
            }
            );
            
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "OnlineSubsystem",
                "OnlineSubsystemUtils"
            }
            );
            
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "Projects",
                "HTTP",
                "Json"
            }
            );
        
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
            );

    }
}
