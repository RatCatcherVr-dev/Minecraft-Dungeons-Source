#include "DungeonsClientInfo.h"
#include "ClientStringUtil.h"

DungeonsClientInfo::DungeonsClientInfo(const FString& platform, const FString& version, const FString& build)
	: Platform(platform), Version(version), Build(build) {
}

minecraft::api::ClientInfo DungeonsClientInfo::ToClientInfo() const {
	using namespace dungeonsapiclient::utils;
	return minecraft::api::ClientInfo(toString(GetClientName()), toString(Version), toString(Build));
}

FString DungeonsClientInfo::GetClientName() const {
	const auto name = "Dungeons/" + Platform;
	return name.ToLower();
}
