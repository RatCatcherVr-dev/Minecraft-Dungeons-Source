#pragma once

#include "CoreMinimal.h"
#include "CosmeticType.generated.h"

UENUM(BlueprintType)
enum class ECosmeticType : uint8 {
	Unset,
	Cape,
	Pet,
	Invalid
};
ENUM_NAME(ECosmeticType);

namespace game { namespace cosmetics {	
	ECosmeticType fromString(const FString& cosmeticType);
	ECosmeticType fromString(const std::string& cosmeticType);
	TArray<ECosmeticType> getEquippableTypes();
}}