
#include "Dungeons.h"
#include "Misc/EngineBuildSettings.h"
#include "Core/Public/GenericPlatform/GenericPlatformCrashContext.h"
#include "platform/GameVersion.h"
#include "platform/DungeonsVersion.h"
#include "online/entitlements/Entitlement.h"
#include "util/telemetry/Analytics.h"

#include "DungeonsVersionContext.h"

// review needed includes.

#ifndef NOINITCRASHREPORTER
#define NOINITCRASHREPORTER 1
#endif

/**
*	FDungeonsVersionContext
*/
namespace dungeonsversion {
	const FString BuildTypeLauncher = TEXT("Launcher");
	const FString BuildTypeWin10Store = TEXT("Win10Store");
	const FString BuildTypeEpicStore = TEXT("EpicStore");
	const FString BuildTypeUndefined = TEXT("BuildTypeUndefined");
	const FString CrashReportBackendSentryKey = TEXT("__sentry");

	/** Helper to get the standard string for the crash type based on crash event bool values. */
	const FString& GetDungeonsBuildTypeString(EEntitlementsSource Type)
	{
		switch (Type)
		{
		case EEntitlementsSource::MinecraftNet:
			return BuildTypeLauncher;
		case EEntitlementsSource::WindowsStore:
			return BuildTypeWin10Store;
		case EEntitlementsSource::EpicStore:
			return BuildTypeEpicStore;
		default:
			return BuildTypeUndefined;
		}
	}

	const FString DungeonsCrashReportDataJson() {
		// Note! The keys used for _SENTRY_ data are parsed 
		// name for name by the backend. 
		// free form keys will be uploaded inside the XML, but _not_ parsed by Sentry.
		// See the documentation online for details https://develop.sentry.dev/sdk/event-payloads/contexts/#app-context 
		Json::Value root;
		std::string release_name = "dungeons@";
		root["release"] = release_name += GAME_VERSION;
		
		Json::Value contexts(Json::objectValue);
		Json::Value app(Json::objectValue);
		app["build_type"] = std::string(TCHAR_TO_UTF8(*dungeonsversion::GetDungeonsBuildTypeString(online::entitlements::getPrimaryEntitlementsSource())));

		contexts["app"] = app;
		root["contexts"] = contexts;

		Json::Value tags(Json::objectValue);
		tags["build_type"] = app["build_type"];
		tags["app_session_id"] = std::string(TCHAR_TO_UTF8(*analytics::Analytics::GetInstance().AppSessionId()));
		root["tags"] = tags;

		Json::StyledWriter writer;
		return FString(writer.write(root).c_str());
	}

	void AddDungeonsVersionToCrashReport() {
		// do add to GameData section of GenericPlatformCrashContext
#if defined ENGINE_WITH_CRASHREPORT_CB
		FGenericCrashContext::AddGameData(CrashReportBackendSentryKey, DungeonsCrashReportDataJson());
#endif
	}
}

const ANSICHAR* FDungeonsVersionContext::DungeonsGameXMLNameA = "DungeonsGame.runtime-xml";
const TCHAR* FDungeonsVersionContext::DungeonsGameXMLNameW = TEXT("DungeonsGame.runtime-xml");
const FString FDungeonsVersionContext::DungeonsPropertiesTag = TEXT("DungeonsProperties");

bool FDungeonsVersionContext::bIsInitialized = false;

namespace NCachedDungeonsContextProperties
{
	static bool bIsInternalBuild;
	static FString PlatformNameIni;
	static FString CrashGUIDRoot;
}

FDungeonsVersionContext::FDungeonsVersionContext(EEntitlementsSource InType, const TCHAR* ErrorMessage) 
	: BuildType(InType)
{
	CommonBuffer.Reserve(32768);
}

void FDungeonsVersionContext::Initialize()
{
#if !NOINITCRASHREPORTER

	NCachedDungeonsContextProperties::bIsInternalBuild = FEngineBuildSettings::IsInternalBuild();
	NCachedDungeonsContextProperties::PlatformNameIni = FPlatformProperties::IniPlatformName();

	NCachedDungeonsContextProperties::CrashGUIDRoot = FGenericCrashContext::GetCrashOutputPath();
		
	bIsInitialized = true;
#endif	// !NOINITCRASHREPORTER
}

FDungeonsVersionContext& FDungeonsVersionContext::Get() {
	static FDungeonsVersionContext sInstance(online::entitlements::getPrimaryEntitlementsSource(), TEXT("Test-message string"));
	return sInstance;
}

void FDungeonsVersionContext::SaveDungeonsVersion() {
	// will not be able to find a crashGUID which is generated at crash time!
	// store in parent for now.
	FString CrashFolder = FPaths::Combine(*FPaths::ProjectSavedDir(), TEXT("Crashes"));
	FString CrashFolderAbsolute = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*CrashFolder);
	if (IFileManager::Get().MakeDirectory(*CrashFolderAbsolute, true))
	{
		// Save Dungeons Version file
		const FString VersionFileXMLPath = FPaths::Combine(*CrashFolderAbsolute, DungeonsGameXMLNameW);
		FDungeonsVersionContext::Get().SerializeAsXML(*VersionFileXMLPath);
	}
}

void FDungeonsVersionContext::SerializeContentToBuffer() const
{
	// to call in Initialize
#if !NOINITCRASHREPORTER
	NCachedDungeonsContextProperties::bIsInternalBuild = FEngineBuildSettings::IsInternalBuild();
#endif
	// Must conform against:
	// https://www.securecoding.cert.org/confluence/display/seccode/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers
	AddHeader();

	BeginSection( *DungeonsPropertiesTag );

	AddCrashProperty(TEXT("BuildType"), *dungeonsversion::GetDungeonsBuildTypeString(BuildType));
	AddCrashProperty(TEXT("BuildVersion"), UGameVersion::BuildVersion());

	EndSection( *DungeonsPropertiesTag );
	AddFooter();
}

void FDungeonsVersionContext::SerializeAsXML(const TCHAR* Filename) const {

	SerializeContentToBuffer();
	// Use OS build-in functionality instead.
	FFileHelper::SaveStringToFile(CommonBuffer, Filename, FFileHelper::EEncodingOptions::AutoDetect);
}

void FDungeonsVersionContext::AddHeader() const
{
	CommonBuffer += TEXT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") LINE_TERMINATOR;
	BeginSection(TEXT("FDungeonsVersionContext"));
}

void FDungeonsVersionContext::AddFooter() const
{
	EndSection(TEXT("FDungeonsVersionContext"));
}

void FDungeonsVersionContext::BeginSection(const TCHAR* SectionName) const
{
	CommonBuffer += TEXT("<");
	CommonBuffer += SectionName;
	CommonBuffer += TEXT(">");
	CommonBuffer += LINE_TERMINATOR;
}

void FDungeonsVersionContext::EndSection(const TCHAR* SectionName) const
{
	CommonBuffer += TEXT("</");
	CommonBuffer += SectionName;
	CommonBuffer += TEXT(">");
	CommonBuffer += LINE_TERMINATOR;
}

void FDungeonsVersionContext::AddCrashProperty(const TCHAR* PropertyName, const TCHAR* PropertyValue) const
{
	CommonBuffer += TEXT("<");
	CommonBuffer += PropertyName;
	CommonBuffer += TEXT(">");


	AppendEscapedXMLString(CommonBuffer, PropertyValue);

	CommonBuffer += TEXT("</");
	CommonBuffer += PropertyName;
	CommonBuffer += TEXT(">");
	CommonBuffer += LINE_TERMINATOR;
}

void FDungeonsVersionContext::AppendEscapedXMLString(FString& OutBuffer, const TCHAR* Text)
{
	if (!Text)
	{
		return;
	}

	while (*Text)
	{
		switch (*Text)
		{
		case TCHAR('&'):
			OutBuffer += TEXT("&amp;");
			break;
		case TCHAR('"'):
			OutBuffer += TEXT("&quot;");
			break;
		case TCHAR('\''):
			OutBuffer += TEXT("&apos;");
			break;
		case TCHAR('<'):
			OutBuffer += TEXT("&lt;");
			break;
		case TCHAR('>'):
			OutBuffer += TEXT("&gt;");
			break;
		case TCHAR('\r'):
			break;
		default:
			OutBuffer += *Text;
		};

		Text++;
	}
}

