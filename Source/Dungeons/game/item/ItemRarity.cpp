#include "Dungeons.h"
#include "ItemRarity.h"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"

namespace game { namespace item { namespace rarity {

	EItemRarity fromString(const FString& rarity) {
		return EnumValueFromString(EItemRarity, rarity).Get(EItemRarity::Common);	
	}

	EItemRarity fromString(const std::string& rarity) {
		return fromString(stringutil::toFString(rarity));
	}

}}}
