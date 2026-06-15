#include "Dungeons.h"
#include "EntitledCosmetic.h"

FEntitledCosmetic::FEntitledCosmetic(const FName cosmeticId, const FEntitlement entitlement)
	: CosmeticId(cosmeticId)
	, Entitlement(entitlement) {	
}
