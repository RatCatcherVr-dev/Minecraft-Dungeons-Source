// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System.IO;

using UnrealBuildTool;

public class DungeonsEditor : ModuleRules {
    public DungeonsEditor(ReadOnlyTargetRules Target) : base(Target) {
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
            "LevelSequence",
            "PropertyEditor",
            "BlueprintGraph",
            "KismetCompiler",
            "AssetRegistry",
            "GraphEditor",
        });
        PrivatePCHHeaderFile = "Public/DungeonsEditor.h";
    }
}
