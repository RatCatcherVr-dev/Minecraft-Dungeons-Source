#pragma once
#include "UObject/ObjectMacros.h"
#include "common_header.h"

class FString;

UENUM(BlueprintType)
enum class EItemRarity : uint8 {
	Common,
	Rare,
	Unique
};
ENUM_NAME(EItemRarity);

namespace game { namespace item { namespace rarity {

	EItemRarity fromString(const FString& rarity);
	EItemRarity fromString(const std::string& rarity);

}}}
