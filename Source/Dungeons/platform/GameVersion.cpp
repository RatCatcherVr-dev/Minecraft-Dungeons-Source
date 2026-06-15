
#include "Dungeons.h"
#include "GameVersion.h"
#include "DungeonsVersion.h"
#include "Engine.h"
#if PLATFORM_PS4
 #include <kernel_cpumode_platform.h>	//D11:MR - Detect PS5
#endif
/**
*
*/

FString UGameVersion::BuildVersion()
{
	const char* game_version = GAME_VERSION;
	FString version = FString(game_version);
	UE_LOG(LogDungeons, Verbose, TEXT("setting Build version %s"), *version)
	return version;
}

FString UGameVersion::GetBuildNumber() {
	const auto& buildVersion = BuildVersion();
	auto ind = 0;
	if (buildVersion.FindChar(':', ind)) {
		return buildVersion.Left(ind);
	}
	return buildVersion;
}

FString UGameVersion::GetVersionNumber() {
	FString version;
	if (!GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion")
		, version, GGameIni))
	{
		UE_LOG(LogDungeons, Warning, TEXT("missing Key 'ProjectVersion' in [%s]."), *GGameIni);
	}

	UE_LOG(LogDungeons, Verbose, TEXT("setting Project version %s"), *version)
	return version;
}

EPlatformType UGameVersion::GetPlatformEnum()
{
#if PLATFORM_WINDOWS 
	return EPlatformType::E_PLATFORM_PC;
#elif PLATFORM_PS4
	return EPlatformType::E_PLATFORM_PS4;
#elif PLATFORM_XBOXONE
	return EPlatformType::E_PLATFORM_XBONE;
#elif PLATFORM_MAC
	return EPlatformType::E_PLATFORM_MAC;
#elif PLATFORM_IOS
	return EPlatformType::E_PLATFORM_IOS;
#elif PLATFORM_ANDROID
	return EPlatformType::E_PLATFORM_ANDROID;
#elif PLATFORM_LINUX
	return EPlatformType::E_PLATFORM_LINUX;
#elif PLATFORM_SWITCH
	return EPlatformType::E_PLATFORM_SWITCH;
#else
	return EPlatformType::E_PLATFORM_PC;
#endif
}

ESubPlatformType UGameVersion::GetSubPlatformEnum()
{
	static ESubPlatformType s_SubPlatformVal = (ESubPlatformType )-2;

	if (s_SubPlatformVal == (ESubPlatformType )-2)
	{
		//initialize subplatform val
#if PLATFORM_XBOXONE

		switch (FXboxOneMisc::GetConsoleType())
		{
		case EXboxOneConsoleType::XboxOne:
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_BASE;
			break;
		case EXboxOneConsoleType::XboxOneS:
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_XBONE_S;
			break;
		case EXboxOneConsoleType::Scorpio:
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_XBONE_X;
			break;
		case EXboxOneConsoleType::Lockhart:
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_XBONE_LOCKHART;
			break;
		case EXboxOneConsoleType::Series_X:
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_XBONE_SERIES_X;
			break;
		default:
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_BASE;
			break;
		}


#elif PLATFORM_PS4

		if (sceKernelIsProspero())
		{
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_PS5;
		}
		else if (sceKernelIsNeoMode())
		{
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_PS4_PRO;
		}
		else
		{
			s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_BASE;
		}

#else
		s_SubPlatformVal = ESubPlatformType::E_SUBPLATFORM_BASE;
#endif

	}

	return s_SubPlatformVal;
}

EConfigurationType UGameVersion::GetBuildConfiguration()
{
#if UE_BUILD_DEBUG
	return EConfigurationType::E_CONFIG_DEBUG;
#elif UE_BUILD_DEVELOPMENT
	return EConfigurationType::E_CONFIG_DEVELOPMENT;
#elif UE_BUILD_TEST
	return EConfigurationType::E_CONFIG_TEST;
#elif UE_BUILD_SHIPPING
	return EConfigurationType::E_CONFIG_SHIPPING;
#endif

	
}

bool UGameVersion::GetConfigurationMatch(int32 ConfigFlags)
{
#if UE_BUILD_DEBUG 
	return ((ConfigFlags & (1 << (int32)EConfigurationType::E_CONFIG_DEBUG)) > 0);
#elif UE_BUILD_DEVELOPMENT
	return ((ConfigFlags & (1 << (int32)EConfigurationType::E_CONFIG_DEVELOPMENT)) > 0);
#elif UE_BUILD_TEST
	return ((ConfigFlags & (1 << (int32)EConfigurationType::E_CONFIG_TEST)) > 0);
#elif UE_BUILD_SHIPPING
	return ((ConfigFlags & (1 << (int32)EConfigurationType::E_CONFIG_SHIPPING)) > 0);
#else
	return false;
#endif
}

void UGameVersion::DungeonsPrintString(UObject* WorldContextObject, const FString& InString /*= FString(TEXT("Hello"))*/, bool bPrintToScreen /*= true*/, FLinearColor TextColor /*= FLinearColor(0.0, 0.66, 1.0)*/, float Duration /*= 2.f*/)
{

	FString FinalLogString = InString;

	
	const FString SourceObjectPrefix = FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject));
	FinalLogString = SourceObjectPrefix + FinalLogString;
	
	
	UE_LOG(LogTemp, Log, TEXT("%s"), *FinalLogString);


	// Also output to the screen, if possible
	if (bPrintToScreen)
	{
		GEngine->AddOnScreenDebugMessage((uint64)-1, Duration, TextColor.ToFColor(true), FinalLogString);
	}

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("LowLvl: %s\n"), *FinalLogString);
}


bool UGameVersion::GetPlatformMatch(int32 CheckPlatformFlags)
{
#if PLATFORM_WINDOWS 
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_PC)) > 0);
#elif PLATFORM_PS4
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_PS4)) > 0);
#elif PLATFORM_XBOXONE
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_XBONE)) > 0);
#elif PLATFORM_MAC
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_MAC)) > 0);
#elif PLATFORM_IOS
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_IOS)) > 0);
#elif PLATFORM_ANDROID
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_ANDROID)) > 0);
#elif PLATFORM_LINUX
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_LINUX)) > 0);
#elif PLATFORM_SWITCH
	return ((CheckPlatformFlags & (1 << (int32)EPlatformType::E_PLATFORM_SWITCH)) > 0);
#else
	return false;
#endif
}

bool UGameVersion::GetSubPlatformMatch(int32 PlatformFlags)
{
	return ((PlatformFlags & (1 << (int32)GetSubPlatformEnum())) > 0);
}

namespace DungeonsQA {
	extern TAutoConsoleVariable<int32> CVShowVersionString;
};

FText UGameVersion::GetMenuVersionString()
{
#if !UE_BUILD_SHIPPING
	if (DungeonsQA::CVShowVersionString.GetValueOnGameThread() >= 1) 
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("version-string"), FText::FromString(UGameVersion::BuildVersion()));
		return FText::Format(NSLOCTEXT("version", "versionString", "build # {version-string}"), Arguments);
	}
#endif

	return FText::GetEmpty();
}
