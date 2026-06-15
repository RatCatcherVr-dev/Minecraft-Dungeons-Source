#pragma once

#include "CoreMinimal.h"

struct DUNGEONSAUTH_API XAuthData {	
	XAuthData(const FString& xuid, const FString& playfabToken, const FString& platformtoken, const FString& namespaceLogin);
	
	const FString Xuid;
	const FString PlayfabToken;
	const FString PlatformToken;
	const FString NamespaceLogin;
};