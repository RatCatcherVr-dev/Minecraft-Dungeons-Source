#include "Dungeons.h"
#include "ConfigFileUtil.h"
#include <Themida/Anticheat.hpp>

namespace configfile {

bool IsDemo() {
	if (!GConfig) {
		return false;
	}

	const FString sectionName = "Demo";
	FString DemoSetup;
	GConfig->GetString(*sectionName,
		TEXT("Setup"),
		DemoSetup,
		GGameIni);

	return !DemoSetup.IsEmpty();

}

bool ShowRewardScreen() {
	if (!GConfig) {
		return false;
	}

	const FString sectionName = "Demo";
	bool showRewardScreen;
	GConfig->GetBool(*sectionName,
		TEXT("ShowRewardScreen"),
		showRewardScreen,
		GGameIni);

	return IsDemo() && showRewardScreen;

}

// D11.SSN
bool SkipSplash() {
	if (!GConfig) {
		return false;
	}

	const FString sectionName = "Splash";
	bool skipSplash;
	GConfig->GetBool(*sectionName, TEXT("SkipSplash"), skipSplash, GGameIni);

#if UE_BUILD_SHIPPING
	return false;
#endif
	return skipSplash;
}

// D11.SSN
float SplashTime() {
	if (!GConfig) {
		return false;
	}

	const FString sectionName = "Splash";
	float splashTime;
	GConfig->GetFloat(*sectionName, TEXT("SplashTime"), splashTime, GGameIni);

	return splashTime;
}


TArray<FPlatformEntitlementMapping> EntitlementMappings() {
	TArray<FPlatformEntitlementMapping> parsedMappings;

	if (!GConfig) {
		return parsedMappings;
	}
	ANTICHEAT_PROTECT_STRINGS_BEGIN
	const FString sectionName = "EntitlementMapping";
	const FString propertyEntitlementName = "EntitlementName=";
	const FString propertyPlatformSkuId = "PlatformSkuId=";
	ANTICHEAT_PROTECT_STRINGS_END

		if (GConfig)
		{
			FConfigSection* EntitlementMapping = GConfig->GetSectionPrivate(*sectionName, false, true, GGameIni);
			if (EntitlementMapping)
			{
				for (FConfigSection::TIterator It(*EntitlementMapping); It; ++It)
				{
					FString GameId, PlatformId;
					const FString& ValueString = It.Value().GetValue();
					FParse::Value(*ValueString, *propertyEntitlementName, GameId);
					FParse::Value(*ValueString, *propertyPlatformSkuId, PlatformId);
					parsedMappings.Add({ GameId,PlatformId });
				}
			}
		}

	return parsedMappings;
}


FString GetProductId(const FString &dlcName) {


	if (!GConfig) {
		return FString();
	}

	ANTICHEAT_PROTECT_STRINGS_BEGIN
	const FString sectionName = "ProductIdMapping";
	const FString propertyDlcName = "DlcName=";
	const FString propertyProductId = "PlatformProductId=";
	ANTICHEAT_PROTECT_STRINGS_END

	if (GConfig)
	{
		FConfigSection* EntitlementMapping = GConfig->GetSectionPrivate(*sectionName, false, true, GGameIni);
		if (EntitlementMapping)
		{
			for (FConfigSection::TIterator It(*EntitlementMapping); It; ++It)
			{
				FString name, id;
				const FString& ValueString = It.Value().GetValue();
				FParse::Value(*ValueString, *propertyDlcName, name);
				FParse::Value(*ValueString, *propertyProductId, id);

				if (dlcName == name)
				{
					return id;
				}
			}
		}
	}

	return FString();
}

}
