#pragma once

#include "common_header.h"

UENUM(BlueprintType)
enum class EEyeOfEnderType : uint8 {
	NOT_SET,
	DEEPWOOD_BROOK,
	OLD_TOWN_HALL,
	SUNKEN_WATCHTOWER,
	THE_UNDERCROFT,
	THE_TRIAL_OF_THE_NAMELESS,
	HIGHBLOCK_HIDEAWAY,
	// NOTE: If adding a new value, update eyeOfEnderTypeNames in this .cpp
	ENUM_COUNT
};
ENUM_NAME(EEyeOfEnderType);

//namespace eyeofender {
static const int TOTAL_EYES_OF_ENDER = 6;
//}


namespace eyeofenderquery {
	extern const std::vector<EEyeOfEnderType> AllEyeOfEnderTypes;

	bool isValid(EEyeOfEnderType);
	TOptional<std::string> toString(EEyeOfEnderType);

	TOptional<FString> unlockString(EEyeOfEnderType);
}
