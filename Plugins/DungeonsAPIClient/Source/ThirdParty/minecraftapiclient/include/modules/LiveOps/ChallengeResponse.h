#pragma once

#include "CoreMinimal.h"
#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "jsonutil.h"
#include "RewardResponse.h"
#include <map>

namespace minecraft {
namespace api {

enum class ObjectiveType {
	MobDeath,
	MissionCompleted,
	XpGained,
	PlayerHealed,
	ReviveFriend,
	CurrencyCollected,
	TrialComplete,
	ItemUsed,
	ChestOpened,
	BlocksTravelled
};

enum class FilterPartType {
	Mob,
	MobTag,
	Operator,
	Mission,
	MinDifficulty,
	MinEndlessStruggle,
	Item,
	ItemTag,
	Currency,
	Player,
	Artifact,
	Potion,
	Enchantment,
	TrialType,
};


namespace enumconversion {

// @attn: We don't access enumutil from this module. We should create a tools-module with general stuff like that.	
const auto objectiveStringToEnum = [](const std::string& type) -> TOptional<ObjectiveType> {
	if (lowerEquals(type, "MobDeath"))
		return ObjectiveType::MobDeath;
	if (lowerEquals(type, "MissionCompleted"))
		return ObjectiveType::MissionCompleted;
	if (lowerEquals(type, "XpGained"))
		return ObjectiveType::XpGained;
	if (lowerEquals(type, "PlayerHealed"))
		return ObjectiveType::PlayerHealed;
	if (lowerEquals(type, "ReviveFriend"))
		return ObjectiveType::ReviveFriend;
	if (lowerEquals(type, "CurrencyCollected"))
		return ObjectiveType::CurrencyCollected;
	if (lowerEquals(type, "TrialComplete"))
		return ObjectiveType::TrialComplete;
	if (lowerEquals(type, "ItemUsed"))
		return ObjectiveType::ItemUsed;
	if (lowerEquals(type, "ChestOpened"))
		return ObjectiveType::ChestOpened;
	if (lowerEquals(type, "BlocksTravelled"))
		return ObjectiveType::BlocksTravelled;
	return {};
};

const auto filterPartStringToEnum = [](const std::string& type) -> TOptional<FilterPartType> {
	if (lowerEquals(type, "Mob"))
		return FilterPartType::Mob;
	if (lowerEquals(type, "MobTag"))
		return FilterPartType::MobTag;
	if (lowerEquals(type, "Operator"))
		return FilterPartType::Operator;
	if (lowerEquals(type, "Mission"))
		return FilterPartType::Mission;
	if (lowerEquals(type, "MinDifficulty"))
		return FilterPartType::MinDifficulty;
	if (lowerEquals(type, "MinEndlessStruggle"))
		return FilterPartType::MinEndlessStruggle;
	if (lowerEquals(type, "Item"))
		return FilterPartType::Item;
	if (lowerEquals(type, "ItemTag"))
		return FilterPartType::ItemTag;
	if (lowerEquals(type, "Player"))
		return FilterPartType::Player;
	if (lowerEquals(type, "Currency"))
		return FilterPartType::Currency;
	if (lowerEquals(type, "Artifact"))
		return FilterPartType::Artifact;
	if (lowerEquals(type, "Potion"))
		return FilterPartType::Potion;
	if (lowerEquals(type, "Enchantment"))
		return FilterPartType::Enchantment;
	if (lowerEquals(type, "TrialType"))
		return FilterPartType::TrialType;
	return {};
};

}

struct FilterPartResponse {
	FilterPartType type;
	std::string value;
	
	void fromJson(const Json::Value& j) {
		type = parseRequiredEnum<FilterPartType>(j, "type", enumconversion::filterPartStringToEnum);
		value = parseRequired<std::string>(j, "value");
	}

	bool operator==(const FilterPartResponse& rhs) const {
		return type == rhs.type
			&& value == rhs.value;
	}

	bool operator!=(const FilterPartResponse& rhs) const {
		return !(*this == rhs);
	}
};

struct ObjectiveResponse {
	std::string progressName;
	ObjectiveType type;
	std::vector<FilterPartResponse> filterParts;
	int64 requiredScore;
	
	void fromJson(const Json::Value& j) {
		progressName = parseRequired<std::string>(j, "progressName");
		type = parseRequiredEnum<ObjectiveType>(j, "type", enumconversion::objectiveStringToEnum);
		filterParts = parseRequiredArray<FilterPartResponse>(j, "filterParts");
		requiredScore = parseRequired<int64>(j, "requiredScore");
	}
};

struct ChallengeResponse {
	std::string id;
	std::string name;
	std::string activeFrom;
	std::string activeTo;
	std::vector<ObjectiveResponse> objectives;
	std::vector<RewardId> rewards;

	void fromJson(const Json::Value& j) {
		id = parseRequired<std::string>(j, "id");
		name = parseRequired<std::string>(j, "name");
		activeFrom = parseRequired<std::string>(j, "startTime");
		activeTo = parseRequired<std::string>(j, "endTime");
		objectives = parseRequiredArray<ObjectiveResponse>(j, "objectives");
		rewards = parseRequiredArray<RewardId>(j, "rewards");
	}
};

struct ChallengesResponse {
	std::vector<ChallengeResponse> challenges;
	TOptional<std::string> namespaceLogin;

	void fromJson(const Json::Value& j) {
		challenges = parseRequiredArray<ChallengeResponse>(j, "challenges");
		namespaceLogin = parseOptional<std::string>(j, "namespace");
	}
};

struct AdventurePointReward {
	ObjectiveType type;
	std::vector<FilterPartResponse> filterParts;
	int64 scoreGranted;
	int64 perCompleted;
	void fromJson(const Json::Value& j) {
		type = parseRequiredEnum<ObjectiveType>(j, "type", enumconversion::objectiveStringToEnum);
		filterParts = parseRequiredArray<FilterPartResponse>(j, "filterParts");
		scoreGranted = parseRequired<int64>(j, "scoreGranted");
		perCompleted = parseRequired<int64>(j, "perCompleted");
	}

	bool operator==(const AdventurePointReward& rhs) const {
		return type == rhs.type
			&& filterParts == rhs.filterParts
			&& scoreGranted == rhs.scoreGranted
			&& perCompleted == rhs.perCompleted;
	}

	bool operator!=(const AdventurePointReward& rhs) const {
		return !(*this == rhs);
	}
};

struct AdventurePointsConfig {
	std::vector<AdventurePointReward> rewardSources;
	void fromJson(const Json::Value& j) {
		rewardSources = parseRequiredArray<AdventurePointReward>(j, "rewardSources");
	}

	bool operator==(const AdventurePointsConfig& rhs) const {
		return rewardSources == rhs.rewardSources;
	}

	bool operator!=(const AdventurePointsConfig& rhs) const {
		return !(*this == rhs);
	}
};

struct ConfigData {
	AdventurePointsConfig adventurePointsConfig;

	void fromJson(const Json::Value& j) {
		adventurePointsConfig = parseRequired<AdventurePointsConfig>(j, "adventurePointsConfiguration");
	}

	bool operator==(const ConfigData& rhs) const {
		return adventurePointsConfig == rhs.adventurePointsConfig;
	}

	bool operator!=(const ConfigData& rhs) const {
		return !(*this == rhs);
	}
};

struct GameConfigResponse {
	ConfigData data;
	std::string version;
	std::string id;
	TOptional<std::string> namespaceLogin;

	void fromJson(const Json::Value& j) {
		data = parseRequired<ConfigData>(j, "data");
		version = parseRequired<std::string>(j, "version");
		id = parseRequired<std::string>(j, "id");
		namespaceLogin = parseOptional<std::string>(j, "namespace");
	}

	bool operator==(const GameConfigResponse& rhs) const {
		return data == rhs.data
			&& version == rhs.version
			&& id == rhs.id
			&& namespaceLogin == rhs.namespaceLogin;
	}

	bool operator!=(const GameConfigResponse& rhs) const {
		return !(*this == rhs);
	}
};

}
}

