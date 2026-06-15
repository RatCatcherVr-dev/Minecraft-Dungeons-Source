#pragma once

#include "CoreMinimal.h"
#include "core/Config.h"

class DUNGEONSHTTP_API DungeonsHTTPConfig {
public:

	explicit DungeonsHTTPConfig(minecraft::api::Config);
	virtual ~DungeonsHTTPConfig() = default;

	float GetRequestTimeoutInSeconds() const;

	minecraft::api::Config GetMinecraftAPIConfig() const;

	static shared_ptr<DungeonsHTTPConfig> CreateHTTPConfig();
private:	
	minecraft::api::Config Config;
};

class DUNGEONSHTTP_API DevelopmentConfig final : public DungeonsHTTPConfig {

public:
	DevelopmentConfig();
};

class DUNGEONSHTTP_API ProductionConfig final : public DungeonsHTTPConfig {

public:
	ProductionConfig();
};
