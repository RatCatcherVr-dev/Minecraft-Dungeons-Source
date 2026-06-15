#pragma once

#include "CoreMinimal.h"
#include "core/ClientInfo.h"

struct DUNGEONSCLIENT_API DungeonsClientInfo {

	DungeonsClientInfo(const FString& platform, const FString& version, const FString& build);

	minecraft::api::ClientInfo ToClientInfo() const;
	FString GetClientName() const;
	
	const FString Platform;
	const FString Version;
	const FString Build;
};