#include "Dungeons.h"
#include "CosmeticType.h"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"

namespace game { namespace cosmetics {
	ECosmeticType fromString(const FString& cosmeticType) {
		return EnumValueFromString(ECosmeticType, cosmeticType).Get(ECosmeticType::Invalid);
	}
	
	ECosmeticType fromString(const std::string& cosmeticType) {
		return fromString(stringutil::toFString(cosmeticType));
	}

	TArray<ECosmeticType> getEquippableTypes() {
		return {ECosmeticType::Cape, ECosmeticType::Pet};
	}
}}
