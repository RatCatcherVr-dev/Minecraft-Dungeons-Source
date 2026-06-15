//------------------------------------------------------------------------------
// <copyright file="TdkRpc.Build.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using UnrealBuildTool;

/// <summary>
/// Tells the Unreal Build Tool (UBT) how to build this module.
/// </summary>
public class TdkRpc : ModuleRules
{
    /// <summary>
    /// Initializes a new instance of the <see cref="TdkRpc"/> class.
    /// Called by UBT when this module is built.
    /// </summary>
    /// <param name="Target">Data from the Unreal Build Tool.</param>
    public TdkRpc(ReadOnlyTargetRules Target) : base (Target)
    {
        Type = ModuleType.External;

        bEnableUndefinedIdentifierWarnings = false;
        bEnableExceptions = true;

        // Non-shipping Xbox One and Windows x64 builds.
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            string TdkRpcLib = "GamesTest.Rpc.NativeServer.lib";
            string Config = (Target.Configuration == UnrealTargetConfiguration.Debug &&
                             Target.bDebugBuildsActuallyUseDebugCRT) ? "Debug" : "Release";

            PublicIncludePaths.Add(string.Format("{0}/include", ModuleDirectory));

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicLibraryPaths.Add(string.Format("{0}/lib/x64/{1}/", ModuleDirectory, Config));
                PublicAdditionalLibraries.Add(string.Format("{0}/lib/x64/{1}/{2}", ModuleDirectory, Config, TdkRpcLib));
            }
            else if (Target.Platform == UnrealTargetPlatform.XboxOne)
            {
                PublicLibraryPaths.Add(string.Format("{0}/lib/Durango/{1}/", ModuleDirectory, Config));
                PublicAdditionalLibraries.Add(string.Format("{0}/lib/Durango/{1}/{2}", ModuleDirectory, Config, TdkRpcLib));
            }
        }
    }
}
