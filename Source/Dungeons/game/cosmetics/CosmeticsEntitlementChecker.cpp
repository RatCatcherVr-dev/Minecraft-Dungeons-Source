#include "Dungeons.h"
#include "CosmeticsEntitlementChecker.h"
#include "online/entitlements/Entitlement.h"
#include "util/Algo.hpp"
#include "online/entitlements/EntitlementsValidator.h"

namespace game { namespace cosmetics {

	TArray<UCosmeticItemDef*> filterByEntitlements(const TArray<UCosmeticItemDef*>& cosmetics, const TArray<FEntitlement>& entitlements) {
		TArray<UCosmeticItemDef*> filteredCosmetics;
		algo::copy_if(cosmetics, RETLAMBDA(UEntitlementsValidator::HasEntitlement(entitlements, it->GetEntitlementName())), filteredCosmetics);
		return filteredCosmetics;
	}

}}
