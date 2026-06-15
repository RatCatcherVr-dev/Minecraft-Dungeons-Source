#include "Dungeons.h"
#include "game/levels.h"
#include "util/StringUtil.h"

ELevelNames level::fromString(const FString& missionId) {
	return EnumValueFromString(ELevelNames, missionId).Get(ELevelNames::Invalid);	
}

ELevelNames level::fromString(const std::string& missionId) {
	return fromString(stringutil::toFString(missionId));
}
