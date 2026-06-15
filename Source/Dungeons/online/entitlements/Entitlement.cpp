#include "Dungeons.h"
#include "Entitlement.h"

FEntitlement::FEntitlement(const FString name)
	: FEntitlement(name, EEntitlementsSource::Invalid) {
}

FEntitlement::FEntitlement(const FString name, const EEntitlementsSource entitlementsSource)
	: FEntitlement(name, "", "", entitlementsSource) {
}

FEntitlement::FEntitlement(const FString name, const FString signature, const FString signingUserId, const EEntitlementsSource entitlementsSource)
	: Name(name)
	, Signature(signature)
	, SigningUserId(signingUserId)
	, EntitlementsSource(entitlementsSource) {
}

bool FEntitlement::IsSignedBy(const FString userId) const {
	return SigningUserId == userId;
}

bool FEntitlement::IsRoaming() const {
	// Currently, no entitlements should be roaming.
	return false;
}

bool FPlatformEntitlementMapping::IsMatchingPlatformSkuId(const FString& platformSkuId) const {
	return PlatformSkuId == platformSkuId || IsPartiallyMatchingPlatformSkuId(platformSkuId);
}

bool FPlatformEntitlementMapping::IsPartiallyMatchingPlatformSkuId(const FString& platformSkuId) const {
	const auto minLengthToAttemptPartialMatching = 10;
	return PlatformSkuId.Len() > minLengthToAttemptPartialMatching && platformSkuId.StartsWith(PlatformSkuId);
}

namespace online { namespace entitlements {
	EEntitlementsSource getPrimaryEntitlementsSource() {
		#if PLATFORM_WINDOWS
			#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
				//WIN 10 store;
				return EEntitlementsSource::WindowsStore;
			#elif defined(EPIC_STORE_BUILD)
				return EEntitlementsSource::EpicStore;
			#else
				return EEntitlementsSource::MinecraftNet;
			#endif
		#elif PLATFORM_XBOXONE
			return EEntitlementsSource::XBL;
		#elif PLATFORM_SWITCH
			return EEntitlementsSource::Nintendo;
		#elif PLATFORM_PS4
			return EEntitlementsSource::PSN;
		#else
			return EEntitlementsSource::Invalid;
		#endif
	}
}}
