// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System.IO;

using UnrealBuildTool;

public class TileDecorator : ModuleRules {
    public TileDecorator(ReadOnlyTargetRules Target) : base(Target) {
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "EditorStyle",
            "InputCore",
            "Rendercore",
            "RHI",
            "RuntimeMeshComponent",
            "ImageWrapper",
            "UnrealEd",
            "Dungeons",
            "SourceDepsModule",
            "LevelSequence"
        });

        PublicIncludePaths.Add("TileDecorator/Public");
        PrivateIncludePaths.Add("TileDecorator/Private");
		
		PrivatePCHHeaderFile = "Public/TileDecorator.h";
    }
}
