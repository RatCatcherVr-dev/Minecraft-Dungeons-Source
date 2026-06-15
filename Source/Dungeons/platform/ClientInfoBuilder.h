#pragma once

#include "DungeonsClientInfo.h"

class ClientInfoBuilder {

public:
	static DungeonsClientInfo AggregateClientInfo();
	
	void SetPlatform(const FString& platform);
	void SetVersion(const FString& version);
	void SetBuildNumber(const FString& buildNumber);

	DungeonsClientInfo Build() const;
private:
	FString Platform;
	FString Version;
	FString BuildNumber;
};
