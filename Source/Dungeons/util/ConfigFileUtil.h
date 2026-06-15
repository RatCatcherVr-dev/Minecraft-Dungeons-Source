#pragma once
#include "online/entitlements/Entitlement.h"

namespace configfile {
	bool IsDemo();
	bool ShowRewardScreen();
	bool SkipSplash();
	float SplashTime();
	TArray<FPlatformEntitlementMapping> EntitlementMappings();
	FString GetProductId(const FString &dlcName);

}
