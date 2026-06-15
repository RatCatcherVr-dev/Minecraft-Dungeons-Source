#pragma once

#include "jsonutil.h"
#include "RewardResponse.h"

#include <vector>
#include <string>

namespace minecraft {
namespace api {

enum class SeasonType {
	Endless,
	Event,
};

namespace enumconversion
{

const auto seasonTypeStringToEnum = [](const std::string& type) -> TOptional<SeasonType> {
	if (lowerEquals(type, "Endless"))
		return SeasonType::Endless;
	if (lowerEquals(type, "Event"))
		return SeasonType::Event;
	return {};
};

}

struct SeasonLevelRewards {
	std::vector<RewardId> free;
	std::vector<RewardId> paid;

	void fromJson(const Json::Value& j) {
		free = parseRequiredArray<RewardId>(j, "free");
		paid = parseRequiredArray<RewardId>(j, "paid");
	}
};

struct SeasonRewardLevel {
	int64 apRequired;
	SeasonLevelRewards rewards;

	void fromJson(const Json::Value& j) {
		apRequired = parseRequired<int64>(j, "apRequired");
		rewards = parseRequired<SeasonLevelRewards>(j, "rewards");
	}
};

struct Season {
	std::string name;
	std::string displayName;
	SeasonType type;
	std::string activeFrom;
	TOptional<std::string> activeTo;
	std::vector<SeasonRewardLevel> rewardLevels;

	void fromJson(const Json::Value& j) {
		name = parseRequired<std::string>(j, "name");
		displayName = parseRequired<std::string>(j, "displayName");
		type = parseRequiredEnum<SeasonType>(j, "type", enumconversion::seasonTypeStringToEnum);
		activeFrom = parseRequired<std::string>(j, "startTime");
		activeTo = parseOptional<std::string>(j, "endTime");
		rewardLevels = parseRequiredArray<SeasonRewardLevel>(j, "rewardLadder");
	}
};

struct SeasonResponse {
	std::vector<Season> seasons;

	void fromJson(const Json::Value& j) {
		seasons = parseRequiredArray<Season>(j, "configs");
	}
};

}
}
