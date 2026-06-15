#include "Dungeons.h"
#include "EyeOfEnderDefines.h"
#include "util/StringUtil.h"

namespace eyeofenderquery {

const std::vector<EEyeOfEnderType> AllEyeOfEnderTypes = [] {
	std::vector<EEyeOfEnderType> out;
	for (int i = enum_cast(EEyeOfEnderType::NOT_SET) + 1; i < enum_cast(EEyeOfEnderType::ENUM_COUNT); ++i) {
		out.push_back(static_cast<EEyeOfEnderType>(i));
	}
	return out;
}();

const std::unordered_map<EEyeOfEnderType, std::string> eyeOfEnderTypeNames{ {
	{EEyeOfEnderType::DEEPWOOD_BROOK,            "DeepwoodBrook"},
	{EEyeOfEnderType::OLD_TOWN_HALL,             "OldTownHall"},
	{EEyeOfEnderType::SUNKEN_WATCHTOWER,         "SunkenWatchtower"},
	{EEyeOfEnderType::THE_UNDERCROFT,            "TheUndercroft"},
	{EEyeOfEnderType::THE_TRIAL_OF_THE_NAMELESS, "TheTrialOfTheNameless"},
	{EEyeOfEnderType::HIGHBLOCK_HIDEAWAY,        "HighblockHideaway"}
} };

bool isValid(EEyeOfEnderType type) {
	return eyeOfEnderTypeNames.count(type) != 0;
}

TOptional<std::string> toString(EEyeOfEnderType type) {
	const auto& it = eyeOfEnderTypeNames.find(type);
	return it != eyeOfEnderTypeNames.end() ? it->second : TOptional<std::string>{};
}

TOptional<FString> unlockString(EEyeOfEnderType type) {
	if (const auto name = toString(type)) {
		return stringutil::toFString("EnderEye" + name.GetValue());
	}
	return {};
}

}
