// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class LocValidator : ModuleRules
	{
		public LocValidator(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					// ... add other private include paths required here ...
                    "LocValidator/Private"
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core" // from default
                    , "Localization"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ..."
                    "CoreUObject"
                    , "Engine"
                    , "Slate"
                    , "SlateCore"
                    , "EditorStyle"
                    , "LevelEditor"
                    , "Projects"
                }
                );

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}
