#pragma once

#include "game/cosmetics/CosmeticItemDef.h"
#include "online/entitlements/Entitlement.h"

namespace game { namespace cosmetics {
	TArray<UCosmeticItemDef*> filterByEntitlements(const TArray<UCosmeticItemDef*>&, const TArray<FEntitlement>&);
}}
