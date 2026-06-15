#pragma once

#include "jsonutil.h"

namespace minecraft {
namespace api {


enum class RewardTypes {
	Emeralds,
	Gold,
	Item,
	Cosmetic,
	AdventurePoints
};

enum class RewardState {
	Unavailable,
	Locked,
	Unlocked,
	Claimed,
};

namespace enumconversion {

const auto rewardStringToEnum = [](const std::string& type)->TOptional<RewardTypes> {
	if (lowerEquals(type, "Emeralds"))
		return RewardTypes::Emeralds;
	if (lowerEquals(type, "Gold"))
		return RewardTypes::Gold;
	if (lowerEquals(type, "Item"))
		return RewardTypes::Item;
	if (lowerEquals(type, "Cosmetic"))
		return RewardTypes::Cosmetic;
	if (lowerEquals(type, "Adventure_Points"))
		return RewardTypes::AdventurePoints;
	return {};
};

const auto rewardStateToEnum = [](const std::string& type)->TOptional<RewardState> {
	if (lowerEquals(type, "Unavailable"))
		return RewardState::Unavailable;
	if (lowerEquals(type, "Locked"))
		return RewardState::Locked;
	if (lowerEquals(type, "Unlocked"))
		return RewardState::Unlocked;
	if (lowerEquals(type, "Claimed"))
		return RewardState::Claimed;
	return {};
};

}

struct RewardClaimResponse {
	bool succeded;
	std::string message;

	void fromJson(const Json::Value& j) {
		succeded = parseRequired<bool>(j, "succeded");
		message = parseRequired<std::string>(j, "message");
	}
};

struct RewardClaim {
	std::shared_ptr<std::string> rewardId;
	std::shared_ptr<std::string> seasonName;
	std::shared_ptr<std::string> id;

	void toJson(Json::Value &j) {
		writeJson<std::string>(j, "rewardId", rewardId);
		writeJson<std::string>(j, "seasonName", seasonName);
		writeJson<std::string>(j, "id", id);
	}
};

struct RewardClaimRequest {
	std::shared_ptr<std::vector<RewardClaim>> rewards;

	void toJson(Json::Value &j) {
		writeArrayJson<RewardClaim>(j, "rewards", rewards);
	}
};

struct RewardId {
	std::string rewardId;
	RewardTypes type;

	void fromJson(const Json::Value& j) {
		rewardId = parseRequired<std::string>(j, "id");
		type = parseRequiredEnum<RewardTypes>(j, "type", enumconversion::rewardStringToEnum);
	}
};

struct Reward {
	std::string rewardId;
	RewardTypes type;

	TOptional<RewardState> state;
	TOptional<std::string> itemNameId;
	TOptional<int64> value;

	void fromJson(const Json::Value& j) {
		rewardId = parseRequired<std::string>(j, "rewardId");
		type = parseRequiredEnum<RewardTypes>(j, "type", enumconversion::rewardStringToEnum);
		state = parseOptionalEnum<RewardState>(j, "state", enumconversion::rewardStateToEnum);
		itemNameId = parseOptional<std::string>(j, "itemId");
		value = parseOptional<int64>(j, "value");
	}
};

struct RewardResponse {
	std::vector<Reward> rewards;

	void fromJson(const Json::Value& j) {
		rewards = parseRequiredArray<Reward>(j, "rewards");
	}
};

}
}
