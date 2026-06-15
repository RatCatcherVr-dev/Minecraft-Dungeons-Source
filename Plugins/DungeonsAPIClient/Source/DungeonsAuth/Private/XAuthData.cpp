#include "XAuthData.h"

XAuthData::XAuthData(const FString& xuid, const FString& playfabToken, const FString& platformtoken, const FString& namespaceLogin)
	: Xuid(xuid), PlayfabToken(playfabToken), PlatformToken(platformtoken), NamespaceLogin(namespaceLogin) {
}
