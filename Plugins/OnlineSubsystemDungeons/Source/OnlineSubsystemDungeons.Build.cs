using UnrealBuildTool;
using System.IO;
using System;
using System.Reflection;

public class OnlineSubsystemDungeons : ModuleRules
{
    string LibrariesDir;
    string SourceDir;
    string LibFormat;
    string RunningPlatform;
    string CompileMode;
    string LibEnding;

    public OnlineSubsystemDungeons(ReadOnlyTargetRules Target) : base(Target)
    {
        SetupCommonDefinitions();
        SetupPaths();
        SetupCommonThirdPartyLibraries();

        switch (Target.Platform)
        {
            case UnrealTargetPlatform.Win32:
            case UnrealTargetPlatform.Win64:
                SetupWindowsSettings();
                break;
            case UnrealTargetPlatform.XboxOne:
                SetupXboxOneSettings();
                break;
            case UnrealTargetPlatform.Switch:
                SetupSwitchSettings();
                break;
            case UnrealTargetPlatform.PS4:
                SetupPS4Settings();
                break;
            default:
                throw new BuildException("Unsupported target platform for OnlineSubsystemDungeons.");
        }
    }

    public void SetupCommonDefinitions()
    {
        // D11.PC
        PublicDefinitions.Add("ONLINESUBSYSTEMDUNGEONS_ENABLED=1");
        PublicDefinitions.Add("ONLINESUBSYSTEMDUNGEONS_PACKAGE=1");
        PrivatePCHHeaderFile = "Private/PCHOnlineDungeonsSubsystem.h";

        PrivateDependencyModuleNames.AddRange(
    new string[] {
                "Core",
                "Dungeons",
                "CoreUObject",
                "Engine",
                "Sockets",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "Json",
                "Networking",
                "PlayfabServices",
                "TEXTTOSPEECH",
                "HTTP",
                "WebBrowser"
            }
			);

        PrivateIncludePaths.AddRange(
            new string[]{
                "Private/XMPP",
                "Private/XMPP/XMPPModule",
                "Private/XMPP/XMPPModule/XmppStrophe"
            }
            );

        PublicDefinitions.AddRange(new string[] {
                    "WITH_XMPP_STROPHE=1",
                    "WITH_XMPP_JINGLE=0"
                    }
);

        bEnableExceptions = true;
    }

    public void SetupPaths()
    {
        string BaseDir = ModuleDirectory;
        LibrariesDir = BaseDir + "/../../../Source/SourceDepsModule/Libraries/";
        SourceDir = BaseDir + "/../../../SourceDeps/";
        LibFormat = "";
        RunningPlatform = "";
        CompileMode = "";
        LibEnding = "";

        RunningPlatform = Target.Platform.ToString();

        /* Platform Setup */
        switch (Target.Platform)
        {
            case UnrealTargetPlatform.Win32:
            case UnrealTargetPlatform.Win64:
                RunningPlatform = "Win64";  //Override
                LibFormat = ".lib";
                CompileMode = "Release";    //Temporary, xsapi can only be compiled in release. Symbols are always included for now.
                LibEnding = "RelWI64";
                break;
            case UnrealTargetPlatform.XboxOne:
                RunningPlatform = "Durango";  //Override
                LibFormat = ".lib";
                CompileMode = "Release";
                break;
            case UnrealTargetPlatform.Switch:
                RunningPlatform = "Switch";  //Override
                LibFormat = ".a";
                if (Target.Configuration == UnrealTargetConfiguration.Debug)
                {
                    CompileMode = "Debug";
                    LibEnding = "DbgNX64D";
                }
                else
                {
                    CompileMode = "Release";
                    LibEnding = "RelNX64";
                }
                break;
            case UnrealTargetPlatform.PS4:
                RunningPlatform = "Orbis";  //Override
                LibFormat = ".a";
                if (Target.Configuration == UnrealTargetConfiguration.Debug)
                {
                    CompileMode = "Debug";
                    LibEnding = "DbgORBISD";
                }
                else
                {
                    CompileMode = "Release";
                    LibEnding = "RelORBIS";
                }
                break;
            default:
                throw new BuildException("Unsupported target platform for OnlineSubsystemDungeons.");
        }
    }

    public void SetupCommonThirdPartyLibraries()
    {
        AddLibstropheDependency();

        /* Common Headers Platform Specifics*/
        PublicIncludePaths.Add(SourceDir);
        PublicIncludePaths.Add(SourceDir + "xsapi");
        PublicIncludePaths.Add(SourceDir + "xsapi/xsapi");
        PublicIncludePaths.Add(SourceDir + "xsapi/xsapi-c");
        PublicIncludePaths.Add(SourceDir + "xsapi/cpprestinclude");
        PublicIncludePaths.Add(SourceDir + "xsapi/libHttpClient");
        PublicIncludePaths.Add(SourceDir + "xsapi/libHttpClient/httpClient");
        PublicIncludePaths.Add(SourceDir + "xsapi/xal");
        PublicIncludePaths.Add(SourceDir + "xsapi/cll");
        PublicIncludePaths.Add(SourceDir + "Themida");
        PublicIncludePaths.Add(SourceDir + "webrtc");
        PublicIncludePaths.Add(SourceDir + "webrtc/third_party/abseil-cpp");
    }

    public void SetupWindowsSettings()
    {
        PublicDefinitions.AddRange(new string[] {

                    "_NO_XSAPIIMP",
                    "_NO_PPLXIMP",
                    "XSAPI_WIN32",
                    "_NO_ASYNCRTIMP",
                    "WEBRTC_WIN",
                    "_WIN32",
                    "NDEBUG",
                    "_CRT_SECURE_NO_WARNINGS"
                    }
        );

        AddWin32Libraries();

        if (Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicAdditionalLibraries.Add(LibrariesDir + "Themida/SecureEngineSDK64.lib");
            PublicDefinitions.Add("USING_ANTICHEAT=1");
        }

        RuntimeDependencies.Add("$(ProjectDir)/Config/OSS/Windows/*", StagedFileType.UFS);

        PublicDependencyModuleNames.Add("DungeonsAuth");

#if UE4_WITH_CUSTOM_WIN10_SUPPORT
                if (Target.WindowsPlatform.bTargetWin10 == true)
                {
                    PublicDependencyModuleNames.Add("GDK");
                }
#endif
#if UE4_WITH_CUSTOM_EPIC_STORE_SUPPORT
        if (Target.WindowsPlatform.bTargetEpicStore)
        {
            PrivateDependencyModuleNames.Add("EpicServices");
            PrivateDefinitions.Add("EPIC_STORE_BUILD=1");
        }
#endif

#if UE4_WITH_CUSTOM_STEAM_SUPPORT
        if (Target.WindowsPlatform.bTargetSteam)
        {
            PrivateDefinitions.Add("STEAM_BUILD=1");
        }
#endif

    }

    public void SetupXboxOneSettings()
    {
        PublicDefinitions.AddRange(
new string[]
{
                    "_VARIADIC_MAX=10",
                    "ENABLE_INTSAFE_SIGNED_FUNCTIONS",
                    "_NO_ASYNCRTIMP",
                    "_NO_PPLXIMP",
                    "_NO_XSAPIIMP",
                    "XBL_API_NONE",
                    "_TITLE",
                    "MONOLITHIC=1",
                    "WINAPI_FAMILY=WINAPI_FAMILY_TV_TITLE",
                    "_DURANGO",
                    "_XBOX_ONE",
                    "WIN32_LEAN_AND_MEAN",
                    "_UNICODE",
                    "UNICODE",
                    "HC_PLATFORM_MSBUILD_GUESS=HC_PLATFORM_XDK",
                    "WEBRTC_WIN",
});

        AddDurangoLibraries();

        /* Additional Windows/Unreal libraries */
        PublicLibraryPaths.Add(LibrariesDir + "external" + "/" + "XboxOne");

        PublicAdditionalLibraries.Add("libssl.lib");
        PublicAdditionalLibraries.Add("libcrypto.lib");
        PublicAdditionalLibraries.Add("etwplus.lib");

        PublicDependencyModuleNames.Add("WebBrowser");
        AddLibstropheDependency();

        RuntimeDependencies.Add("$(ProjectDir)/Config/OSS/XboxOne/*", StagedFileType.UFS);
    }

    public void SetupSwitchSettings()
    {
        bEnableUndefinedIdentifierWarnings = false;
        bEnableShadowVariableWarnings = false;
        /* Defines */
        PublicDefinitions.AddRange(new string[] {
                    // D11.AH Uses 'C' version
                    "HC_PLATFORM=HC_PLATFORM_GENERIC",
                    "HC_DATAMODEL=HC_DATAMODEL_LP64",
                    "XSAPI_NO_PPL=1",
                    "XSAPI_NOTIFICATION_SERVICE=0",
                    "HC_NOWEBSOCKETS=0",
                    "HC_TRACE_BUILD_LEVEL=2",
                    "WEBRTC_SWITCH"
                    }
        );

        PublicIncludePaths.Add(SourceDir + "external/" + RunningPlatform + "/" + "openssl");

        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "CompactCoreCll.Kinoko-" + LibEnding + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "LibHttpClient.Kinoko-" + LibEnding + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "Xal.Kinoko-" + LibEnding + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "xsapi.kinoko-" + LibEnding + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "webrtc" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "UsrSCTPLib" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "LibSRTP" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "libPosix" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "OpenSSLNX" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "LibCryptoNX" + LibFormat);

        //Lovely reflection to allow us to compile when not having Switch Sdks

        Type switchExports = System.Type.GetType("UnrealBuildTool.SwitchExports, UnrealBuildTool, Version=4.0.0.0, Culture=neutral, PublicKeyToken=null");
        MethodInfo getSdkInstallLocation = switchExports.GetMethod("GetSDKInstallLocation", BindingFlags.Static | BindingFlags.Public);
        MethodInfo getTargetName = switchExports.GetMethod("GetTargetName", BindingFlags.Static | BindingFlags.Public);
        string SDKDir = (string)getSdkInstallLocation.Invoke(null, null);
        string switchTargetName = (string)getTargetName.Invoke(null, null);

        //object switchPlatformObject = switchPlatformField.GetValue(Target);
        Type switchPlatformType = Target.SwitchPlatform.GetType();
        PropertyInfo enableNEXLibraryProp = switchPlatformType.GetProperty("bEnableNEXLibrary");
        bool bEnableNEXLibrary = (bool)enableNEXLibraryProp.GetValue(Target.SwitchPlatform);

        // NEX and Pia are outside of the SDK, so make sure they both exist before we enable the code for it all
        // Pia depends on NEX, so make sure both exist. By default look for ones next to the SDK, not the env var
        // since the env var is too easy to get out of date.
        string NEXDir = Path.Combine(Path.GetDirectoryName(SDKDir), "NintendoSDK-NEX");

        if (!Directory.Exists(NEXDir))
        {
            NEXDir = System.Environment.GetEnvironmentVariable("NINTENDO_SDK_NEX_ROOT");
        }

        bool bHasNEXLibrary = !string.IsNullOrEmpty(NEXDir) && Directory.Exists(NEXDir);
        if (Target.Platform == UnrealTargetPlatform.Switch && !bEnableNEXLibrary)
        {
            bHasNEXLibrary = false;
        }
        if (bHasNEXLibrary)
        {
            PublicDefinitions.Add("WITH_NEX_LIBRARY=1");

            string LibTarget = Target.Platform == UnrealTargetPlatform.Win64 ? "NX-x64-v140" : switchTargetName;
            string NEXLibDir = Path.Combine(NEXDir, "Libraries", LibTarget, "Release");

            string[] Libs = new string[] {
                    "nn_nex_OnlineCore",
                    "nn_nex_LoginClient",
                    "nn_nex_RendezVousClient",
                    "nn_nex_AuthenticationClient",
                    "nn_nex_Ranking2Client",
                    "nn_nex_UtilityClient",
                    "nn_nex_MatchmakingClient",
                    "nn_nex_MatchmakeExtensionClient",
                    "nn_nex_NATTraversalClient",
                    "nn_nex_VSocket",
                    };

            if (Target.Platform == UnrealTargetPlatform.Switch)
            {
                // use the libs directly
                PublicAdditionalLibraries.AddRange(Libs);
            }

            PublicIncludePaths.Add(Path.Combine(NEXDir, "Include"));
            PublicLibraryPaths.Add(NEXLibDir);
        }

        string[] SysLibs = new string[] {
                    "nn_websocket",
                };

        PublicAdditionalLibraries.AddRange(SysLibs);
        RuntimeDependencies.Add("$(ProjectDir)/Config/OSS/Switch/*", StagedFileType.UFS);
    }
    public void SetupPS4Settings()
    {
        bEnableUndefinedIdentifierWarnings = false;
        bEnableShadowVariableWarnings = false;
        /* Defines */
        PublicDefinitions.AddRange(new string[] {
                     "HC_PLATFORM=HC_PLATFORM_GENERIC",
                    "HC_DATAMODEL=HC_DATAMODEL_LP64",
                    "XSAPI_NO_PPL=1",
                    "XSAPI_NOTIFICATION_SERVICE=0",
                    "HC_NOWEBSOCKETS=0",
                    "HC_TRACE_BUILD_LEVEL=2",
                    "WEBRTC_ORBIS",
                    "RAPIDJSON_HAS_CXX11_RVALUE_REFS",
                    "localtime_r=localtime_s"
                    }
        );

        PublicIncludePaths.Add(SourceDir + "external/" + "libPosix");
        PublicIncludePaths.Add(SourceDir + "external/" + "libPosix");

        //PublicIncludePaths.Add(SourceDir + "external/" + "Switch" + "/" + "openssl");
        PrivateIncludePaths.Add("Private/PS4");
        PrivateIncludePaths.Add("Private/PS4/websocketpp");

        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "libCompactCoreCll" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "libHttpClient" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "libXal" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode + "/" + "libXboxLiveServices" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "webrtc" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "UsrSCTPLib" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "LibSRTP" + LibFormat);
        PublicAdditionalLibraries.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode + "/" + "libPosix" + LibFormat);

        //D11.AH  Doesn't like this methods on PS4
        // AddLibstropheDependency();
        PrivateDefinitions.Add("XML_STATIC");
        PrivateDependencyModuleNames.Add("libstrophe");
        PrivateDependencyModuleNames.Add("Expat");
        PrivateDependencyModuleNames.Add("WebSockets");

        RuntimeDependencies.Add("$(ProjectDir)/Config/OSS/PS4/*", StagedFileType.UFS);
    }

    public void AddWin32Libraries()
    {
        /* WebRTC-build*/
        PublicLibraryPaths.Add(LibrariesDir + "webrtc" + "/" + "Release");
        PublicAdditionalLibraries.Add("webrtc" + LibFormat);

        PublicAdditionalLibraries.Add("LibSRTP" + LibFormat);
        PublicAdditionalLibraries.Add("UsrSCTPLib" + LibFormat);

        /* -- XSAPI libraries -- */
        PublicLibraryPaths.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode);
        PublicAdditionalLibraries.Add("Microsoft.Xbox.Services.141.Win32.Cpp" + LibFormat);
        PublicAdditionalLibraries.Add("libHttpClient.141.Win32.C" + LibFormat);
        PublicAdditionalLibraries.Add("Xal.Win32-" + LibEnding + LibFormat);
        PublicAdditionalLibraries.Add("CompactCoreCLL.Win32-" + LibEnding + LibFormat);

        PublicLibraryPaths.Add(EngineDirectory + "/Source/ThirdParty/OpenSSL/1.1.1/lib/" + RunningPlatform + "/VS2015/" + CompileMode);
        PublicAdditionalLibraries.Add("libcrypto.lib");
        PublicAdditionalLibraries.Add("libssl.lib");

        PublicAdditionalLibraries.Add(LibrariesDir + "Zlib/" + "x64" + "/Zlib" + LibFormat);

        /* Additional Windows/Unreal libraries */
        PublicAdditionalLibraries.Add("Winhttp.lib");
        PublicAdditionalLibraries.Add("Version.lib");
        PublicAdditionalLibraries.Add("Crypt32.lib");
        PublicAdditionalLibraries.Add("Secur32.lib");

        PublicAdditionalLibraries.Add(LibrariesDir + "json/" + "x64" + "/json" + LibFormat);
    }

    public void AddDurangoLibraries()
    {
        /* -- XSAPI libraries -- */
        PublicLibraryPaths.Add(LibrariesDir + "xsapi/" + RunningPlatform + "/" + CompileMode);
        PublicAdditionalLibraries.Add("Microsoft.Xbox.Services.141.XDK.Cpp" + LibFormat);
        PublicAdditionalLibraries.Add("libHttpClient.141.XDK.C" + LibFormat);
        if (CompileMode == "Debug")
        {
            PublicAdditionalLibraries.Add("Xal.Xdk-DbgXI64D" + LibFormat);
            PublicAdditionalLibraries.Add("CompactCoreCLL.Xdk-DbgXI64D" + LibFormat);           
        }
        else
        {
            PublicAdditionalLibraries.Add("Xal.Xdk-RelXI64" + LibFormat);
            PublicAdditionalLibraries.Add("CompactCoreCLL.Xdk-RelXI64" + LibFormat);
        }

        /* -- WebRTC libraries -- */
        PublicLibraryPaths.Add(LibrariesDir + "webrtc/" + RunningPlatform + "/" + CompileMode);
        PublicAdditionalLibraries.Add("webrtc" + LibFormat);
        PublicAdditionalLibraries.Add("LibSRTP" + LibFormat);
        PublicAdditionalLibraries.Add("UsrSCTPLib" + LibFormat);
    }

    public void AddLibstropheDependency()
    {
        PrivateDefinitions.Add("XML_STATIC");
        AddEngineThirdPartyPrivateStaticDependencies(Target, "libstrophe");
        AddEngineThirdPartyPrivateStaticDependencies(Target, "Expat");
        PrivateDependencyModuleNames.Add("WebSockets");
    }
}