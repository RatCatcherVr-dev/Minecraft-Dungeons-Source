#include "Dungeons.h"

#include "MinecraftAPIEntitlementsValidator.h"
#include "MinecraftAPIEntitlements.h"
#include "util/StringUtil.h"
#include "util/Algo.hpp"
#include "AuthCommon.h"

namespace online { namespace entitlements { namespace minecraftapi {

		bool matches(const FEntitlement& entitlement, const minecraft::api::ValidatedEntitlement& validatedEntitlement) {
			return entitlement.GetName() == stringutil::toFString(validatedEntitlement.getEntitlementName()) && 
				entitlement.GetSigningUserId() == stringutil::toFString(validatedEntitlement.getSignerId());
		}

		bool isValidated(const FEntitlement& entitlement, const vector<minecraft::api::ValidatedEntitlement>& validatedEntitlements) {
			return algo::any_of(validatedEntitlements, [&](const minecraft::api::ValidatedEntitlement& validatedEntitlement) {
				return matches(entitlement, validatedEntitlement);
			});
		}

		FString getSigningUserId(const FEntitlement& entitlement) {
			return entitlement.GetSigningUserId();
		}

		bool isValidMinecraftAPIEntitlement(const FEntitlement& entitlement) {
			if (const auto entitlements = getEntitlementsClient()) {
				const auto validatedEntitlements = entitlements->Validate(entitlement.GetSignature(), getSigningUserId(entitlement));
				return isValidated(entitlement, validatedEntitlements);
			}
			return false;
		}

		bool isValidEntitlement(const FEntitlement& entitlement) {
			return isValidMinecraftAPIEntitlement(entitlement);			
		}
}}}

bool UMinecraftAPIEntitlementsValidator::IsEntitlementTamperedWith(const FEntitlement& entitlement, const FString expectedEntitlementName, const FString expectedSigningUser) {
	if (Super::IsEntitlementTamperedWith(entitlement, expectedEntitlementName, expectedSigningUser)) {
		return true;
	}
	return !online::entitlements::minecraftapi::isValidEntitlement(entitlement) || !HasCorrectSignature(entitlement, expectedSigningUser);
}

bool UMinecraftAPIEntitlementsValidator::HasCorrectSignature(const FEntitlement& entitlement, const FString expectedSigningUser) const {
	return entitlement.IsSignedBy(expectedSigningUser);
}
