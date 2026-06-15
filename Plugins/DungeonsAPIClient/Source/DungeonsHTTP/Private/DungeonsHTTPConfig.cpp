#include "DungeonsHTTPConfig.h"
#include "ConfigUtil.h"

DungeonsHTTPConfig::DungeonsHTTPConfig(const minecraft::api::Config config) : Config(config) {
}

float DungeonsHTTPConfig::GetRequestTimeoutInSeconds() const {
	return 30;
}

minecraft::api::Config DungeonsHTTPConfig::GetMinecraftAPIConfig() const {
	return Config;
}

shared_ptr<DungeonsHTTPConfig> DungeonsHTTPConfig::CreateHTTPConfig() {
	if (dungeonsapiclient::utils::isDevelopmentMode()) {
		return std::make_shared<DevelopmentConfig>();
	}
	return std::make_shared<ProductionConfig>();
}

DevelopmentConfig::DevelopmentConfig() : DungeonsHTTPConfig(minecraft::api::Config::createForStage()) {
}

ProductionConfig::ProductionConfig() : DungeonsHTTPConfig(minecraft::api::Config::createForProd()) {
}
