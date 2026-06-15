//------------------------------------------------------------------------------
// <copyright file="UnrealToolsFramework.Build.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using UnrealBuildTool;

/// <summary>
/// Tells the Unreal Build Tool (UBT) how to build this module.
/// </summary>
public class UnrealToolsFramework : ModuleRules
{
    /// <summary>
    /// Initializes a new instance of the <see cref="UnrealToolsFramework"/> class.
    /// Called by UBT when this module is built.
    /// </summary>
    /// <param name="Target">Data from the Unreal Build Tool.</param>
    public UnrealToolsFramework(ReadOnlyTargetRules Target) : base (Target)
    {
        bEnableUndefinedIdentifierWarnings = false;
        bEnableExceptions = true;

        // Public and private dependencies are statically linked into the project and
        // are required by public or private code, respectively.
        PublicDependencyModuleNames.AddRange(
            new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "TdkRpc"
        });
		
		PrivatePCHHeaderFile = "Public/UnrealToolsFrameworkPCH.h";
    }
}
