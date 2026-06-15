#include "Dungeons.h"
#include "EntitlementsValidator.h"
#include "util/ConfigFileUtil.h"
#include "util/Algo.hpp"

namespace online { namespace  entitlements {
	bool hasCorrectSource(const FEntitlement entitlement, const EEntitlementsSource primaryEntitlementsSource) {
	#if !UE_BUILD_SHIPPING
		if (entitlement.GetEntitlementsSource() == EEntitlementsSource::Development) {
			return true;
		}
	#endif
		return entitlement.IsRoaming() || entitlement.GetEntitlementsSource() == primaryEntitlementsSource;
	}
	
	TOptional<FEntitlement> findEntitlementFromValidSource(const TArray<FEntitlement>& entitlements, const FString entitlementName, const EEntitlementsSource primaryEntitlementsSource) {
		if (const auto* foundEntitlement = entitlements.FindByPredicate(RETLAMBDA(it.GetName() == entitlementName && hasCorrectSource(it, primaryEntitlementsSource)))) {
			return *foundEntitlement;
		}
		return TOptional<FEntitlement>();
	}

	TOptional<FEntitlement> findEntitlementFromAnySource(const TArray<FEntitlement>& entitlements, const FString entitlementName) {
		if (const auto* foundEntitlement = entitlements.FindByPredicate(RETLAMBDA(it.GetName() == entitlementName))) {
			return *foundEntitlement;
		}
		return TOptional<FEntitlement>();
	}
}}


bool UEntitlementsValidator::IsEntitlementTamperedWith(const FEntitlement& entitlement, const FString expectedEntitlementName, const FString expectedSigningUser) {
	return entitlement.GetName() != expectedEntitlementName;
}

bool UEntitlementsValidator::HasEntitlement(const TArray<FEntitlement>& entitlements, const FString entitlementName) {
	return FindEntitlement(entitlements, entitlementName).IsSet();
}

bool UEntitlementsValidator::HasAnyEntitlement(const TArray<FEntitlement>& entitlements, const TArray<FString>& entitlementNames) {
	return algo::any_of(entitlementNames, RETLAMBDA(HasEntitlement(entitlements, it)));
}

TOptional<FEntitlement> UEntitlementsValidator::FindEntitlement(const TArray<FEntitlement>& entitlements, const FString entitlementName) {
	using namespace online::entitlements;
	return findEntitlementFromAnySource(entitlements, entitlementName);
}

