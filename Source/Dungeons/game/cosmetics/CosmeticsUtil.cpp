#include "Dungeons.h"
#include "CosmeticsUtil.h"
#include "CosmeticsLibrary.h"
#include "DungeonsGameInstance.h"

#define LOCTEXT_NAMESPACE "Cosmetics"

FText UCosmeticsUtil::getCostmeticTypeDisplayName(ECosmeticType cosmeticsType) {
	switch (cosmeticsType) {	
	case ECosmeticType::Cape:
		return LOCTEXT("cosmeticstype_cape", "Cape");
	case ECosmeticType::Pet:
		return LOCTEXT("cosmeticstype_pet", "Pet");
	default:
		return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE