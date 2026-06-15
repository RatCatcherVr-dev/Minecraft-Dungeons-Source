using UnrealBuildTool;

public class BotAutomation : ModuleRules {
    public BotAutomation(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "Dungeons"
        });

        PrivatePCHHeaderFile = "Public/BotAutomationPCH.h";
    }
}
