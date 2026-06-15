#include "ClientInfoBuilder.h"
#include "GameVersion.h"
#include "util/EnumUtil.h"

namespace platform {
	FString getGamePlatformAsString() {

		const auto platform = UGameVersion::GetPlatformEnum();
		if (platform == EPlatformType::E_PLATFORM_PC) {
		#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
			return "WindowsStore";
		#else
			return "WindowsLauncher";
		#endif	
		}

		auto platformAsString = GetEnumValueToString(platform);
		platformAsString.RemoveFromStart(TEXT("E_PLATFORM_"));
		return platformAsString;
	}
}

DungeonsClientInfo ClientInfoBuilder::AggregateClientInfo() {
	auto builder = ClientInfoBuilder();
	builder.SetPlatform(platform::getGamePlatformAsString());
	builder.SetVersion(UGameVersion::GetVersionNumber());
	builder.SetBuildNumber(UGameVersion::GetBuildNumber());
	return builder.Build();	
}

void ClientInfoBuilder::SetPlatform(const FString& platform) {
	Platform = platform;
}

void ClientInfoBuilder::SetVersion(const FString& version) {
	Version = version;
}

void ClientInfoBuilder::SetBuildNumber(const FString& buildNumber) {
	BuildNumber = buildNumber;
}

DungeonsClientInfo ClientInfoBuilder::Build() const {
	return DungeonsClientInfo(Platform, Version, BuildNumber);
}
