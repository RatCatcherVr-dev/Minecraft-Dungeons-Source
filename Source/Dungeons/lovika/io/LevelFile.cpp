#include "Dungeons.h"
#include "LevelFile.h"
#include "IoHyperTypes.h"
#include "game/mission/MissionDefs.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "lovika/world/level/postprocess/PostProcessConfigs.h"
#include "world/entity/EntityTypes.h"
#include "util/Algo.h"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"
#include "util/TrueCounter.h"
#include <json/json.h>
#include <LogMacros.h>
#include <Misc/AssertionMacros.h>
#include <numeric>

#include "save/JsonCommon.h"

namespace io {

struct PropGroupDef : public WeightedId {
	using WeightedId::WeightedId;
	std::vector<WeightedTileId> props;
	std::vector<RegionLocator> targets;
};

using TileGroups = std::unordered_map<std::string, std::vector<WeightedTileId>>;
using PropGroupDefs = std::unordered_map<std::string, PropGroupDef>;
using MobGroups = std::vector<MobGroup>;


struct PrefabDatas {
	const TileGroups& tileGroups;
	const PropGroupDefs& propGroupDefs;
};

template <typename T>
std::vector<T> parseArrayMaybeT(const Json::Value& array, const std::function<TOptional<T>(const Json::Value&)>& parser) {
	const auto elements = algo::map_vector(array, parser);
	ensureMsgf(algo::all_of(elements, RETLAMBDA(it)), TEXT("Some elements could not be parsed!"));
	return algo::opt::unpack_vector(elements);
}

template <typename EnumType>
std::vector<EnumType> parseEnumArrayT(const Json::Value& array) {
	return parseArrayMaybeT<EnumType>(array, RETLAMBDA(GetEnumValueFromStringT<EnumType>(stringutil::toFString(it.asString()))));
}

static TOptional<float> parseFloat(const Json::Value& node) {
	return node.isNumeric() ? node.asFloat() : TOptional<float>();
}

//
// Merging
//
Overrides merge(const Overrides& parent, const Overrides& child) {
	Overrides o;
	o.propDensity = child.propDensity ? child.propDensity : parent.propDensity;
	o.strayPath = merge(parent.strayPath, child.strayPath);
	o.ambience = algo::opt::firstValid(child.ambience, parent.ambience);
	o.audioAmbience = algo::opt::firstValid(child.audioAmbience, parent.audioAmbience);
	return o;
}

StrayPathVariant merge(const StrayPathVariant& parent, const StrayPathVariant& child) {
	return StrayPathVariant(
		child.id,
		child.weight,
		child.maxLength ? child.maxLength : parent.maxLength,
		child.tiles ? child.tiles : parent.tiles,
		child.deadEnds ? child.deadEnds : parent.deadEnds
	);
}

std::vector<StrayPathVariant> mergeVariantsOf(const StrayPathConfig& parent, const StrayPathConfig& child) {
	if (!child.variants && !parent.variants) {
		return { merge(parent.fallback, child.fallback) };
	}
	auto fallback = child.variants ? merge(parent.fallback, child.fallback) : parent.fallback;
	auto variants = child.variants ? child.variants.GetValue() : parent.variants.GetValue();
	for (auto& variant : variants) {
		variant = merge(fallback, variant);
	}
	return variants;
}

StrayPathConfig merge(const StrayPathConfig& parent, const StrayPathConfig& child) {
	auto merged = StrayPathConfig{
		child.probability ? child.probability : parent.probability,
		child.fallback, // Not really needed anymore, so perhaps change out this structure altogether?
		mergeVariantsOf(parent, child),
	};
	return merged;
}

static bool isDefinedAsNull(const Json::Value& node, const std::string& key) {
	return node.isMember(key) && node[key].isNull();
}

static bool isArray(const Json::Value& node) {
	return node.isArray() && !node.isNull();
}

template <typename StringLikeType = std::string>
static TOptional<std::vector<StringLikeType>> asStringArray(const Json::Value& node) {
	if (isArray(node)) {
		return algo::map_vector(node, RETLAMBDA(StringLikeType{ it.asString().c_str() }));
	}
	if (node.isString()) {
		return std::vector<StringLikeType>{ StringLikeType{ node.asCString() } };
	}
	return {};
}

TOptional<std::string> parseFirstResourcePack(const Json::Value& node) {
	const auto resourcePacks = asStringArray(node);
	if (resourcePacks && !resourcePacks->empty()) {
		return resourcePacks->at(0);
	}
	return {};
}

static TOptional<std::vector<int>> asIntArray(const Json::Value& node) {
	if (isArray(node)) {
		return algo::map_vector(node, RETLAMBDA(it.asInt()));
	}
	if (node.isInt()) {
		return std::vector<int>{ node.asInt() };
	}
	return {};
}

static WeightedId parseWeightedId(const Json::Value& node) {
	auto id = node.isString() ? node.asString() : node["id"].asString("");
	return { id, (node.isObject() && node.isMember("weight")) ? node["weight"].asFloat() : 1.0f };
}

static std::vector<WeightedId> parseWeightedIds(const Json::Value& node) {
	return algo::map_vector(node, parseWeightedId);
}

static TOptional<FString> asFString(const Json::Value& node) {
	if (node.isString()) {
		return FString(node.asCString());
	}
	return {};
}

template <typename EnumType>
static TOptional<EnumType> asUEnumValue(const Json::Value& node) {
	if (node.isString()) {
		return GetEnumValueFromStringT<EnumType>(node.asCString());
	}
	return {};
}

static void parseStretchMobs(const Json::Value&, StretchMobs&, const MobGroups&, const StretchMobs& defaultMobs);
static TOptional<std::vector<WeightedTileId>> parseTiles(const Json::Value&, const PrefabDatas&);
static void parseStretchTiles(const Json::Value&, std::vector<WeightedTileId>&, const TileGroups&);
static void parseChallenges(const Json::Value&, std::vector<Challenge>& out, const MobGroups&, const std::string&, const std::vector<Challenge>& challengeRegistry, std::vector<std::string>* outMissingReferencedIds = nullptr);
static void parseObjectiveRegionLocator(const Json::Value&, std::vector<RegionLocator>& out);
static TOptional<RegionLocator> parseSingleObjectiveRegionLocator(const Json::Value&);

static Rotations parseRotation(const Json::Value& node) {
	if (node.isNull()) {
		return {};
	}
	if (node.isIntegral()) {
		return Rotations(std::vector<QuadrantAngle>({ quadrantFromDegrees(node.asInt()) }));
	}
	if (isArray(node)) {
		return Rotations(algo::map_vector(node, RETLAMBDA(quadrantFromDegrees(it.asInt()))));
	}
	return {};
}

//
// Tiles
//
static Teleport parseTeleport(const Json::Value& node) {
	return {
		node["door"].asString(),
		parseSingleObjectiveRegionLocator(node["exit"]),
		node.isMember("dungeons") ? parseWeightedIds(node["dungeons"]) : TOptional<std::vector<WeightedId>>{},
		asFString(node["object"])
	};
}

static void parseTeleports(const Json::Value& group, std::vector<Teleport>& teleports) {
	algo::map_to(group, parseTeleport, teleports);
}

static Tile parseTileDef(const Json::Value& node, const MobGroups& mobGroups, const std::string& loctableId, const std::vector<Challenge>& challengeRegistry) {
	if (node.isString()) {
		return Tile(node.asString());
	}
	Tile tile(
		node["id"].asString(),
		node["weight"].asFloat(1)
	);
	const auto objectSeparatorIndex = tile.id.find('@');
	if (objectSeparatorIndex != std::string::npos) {
		tile.objectId = tile.id.substr(objectSeparatorIndex + 1);
	} else {
		tile.objectId = node["object"].asString(tile.id);
	}
	tile.metadata.rotations = parseRotation(node["rotations"]);
	tile.metadata.entryDoor = node["entry-door"].asString();
	tile.metadata.exitDoors = asStringArray(node["exit-door"]).Get({});
	tile.metadata.unlockKeys = algo::map_vector(asStringArray(node["unlock-keys"]).Get({}), Util::toLower);
	parseChallenges(node["challenges"], tile.metadata.challenges, mobGroups, loctableId, challengeRegistry, &tile.metadata.challengesNotFound);
	parseTeleports(node["teleports"], tile.metadata.teleports);
	return tile;
}

static void parseTileDefs(const Json::Value& node, std::vector<Tile>& tiles, const MobGroups& mobGroups,const std::string& levelId, const std::vector<Challenge>& challengeRegistry) {
	for (const Json::Value& tileNode : node) {
		tiles.push_back(parseTileDef(tileNode, mobGroups, levelId, challengeRegistry));
	}
}

static void parseTiles(const Json::Value& tilesNode, std::vector<WeightedTileId>& tiles) {
	algo::map_to(tilesNode, parseWeightedId, tiles);
}

static void parseTileGroupDefs(const Json::Value& tileGroupsNode, TileGroups& tileGroups) {
	for (const Json::Value& node : tileGroupsNode) {
		std::vector<WeightedTileId> tiles;
		parseTiles(node["tiles"], tiles);
		tileGroups.emplace(Util::toLower(node["id"].asString()), tiles);
	}
}

static void parsePropGroupDefs(const Json::Value& groupsNode, PropGroupDefs& propGroups) {
	for (const Json::Value& node : groupsNode) {
		PropGroupDef def(node["id"].asString());
		parseTiles(node["props"], def.props);
		parseObjectiveRegionLocator(node["on-regions"], def.targets);
		propGroups.emplace(def.lowerId, def);
	}
}

static void _addTiles(std::vector<WeightedTileId>& tiles, const std::string& id, const TileGroups& tileGroups, float weightScale) {
	const auto it = tileGroups.find(Util::toLower(id));
	checkf(it != end(tileGroups), TEXT("Couldn't find tile-group id: %s"), *FString(id.c_str()));

	const auto& group = it->second;
	if (group.empty()) {
		return;
	}
	const auto weightMultiplier = weightScale / algo::sum(group, RETLAMBDA(it.getWeight()));
	for (auto& tile : group) {
		tiles.push_back(tile);
		tiles.back().weight *= weightMultiplier;
	}
}

static void _addProps(std::vector<PropId>& out, const std::string& id, const PropGroupDefs& prefabGroups, float weightScale) {
	const auto it = prefabGroups.find(Util::toLower(id));
	if (it == end(prefabGroups)) {
		ensureMsgf(false, TEXT("Couldn't find prop-group id: %s"), *FString(id.c_str()));
		return;
	}
	const auto& group = it->second;
	if (group.props.empty()) {
		return;
	}
	const auto weightMultiplier = weightScale / algo::sum(group.props, RETLAMBDA(it.getWeight()));
	for (auto& prefab : group.props) {
		out.emplace_back(prefab.id, prefab.weight * weightMultiplier);
		out.back().targets = group.targets;
	}
}

//
// Side-Paths
//
StrayPathVariant parseStrayPathVariant(const Json::Value& node, const PrefabDatas& tileDatas, const std::string& id) {
	StrayPathVariant variant;
	variant.id = id;
	variant.weight = node["weight"].asFloat(1.0f);
	variant.tiles = parseTiles(node, tileDatas);

	auto& maxLength = node["max-length"];
	if (isArray(maxLength)) {
		variant.maxLength = { maxLength[0].asInt(), maxLength[1].asInt() };
	}
	if (node.isMember("dead-ends")) {
		variant.deadEnds.Emplace();
		parseTiles(node["dead-ends"], variant.deadEnds.GetValue());
	}
	return variant;
}

TOptional<std::vector<StrayPathVariant>> parseStrayPathVariants(const Json::Value& node, const PrefabDatas& tileDatas) {
	if (node.isNull()) {
		return {};
	}
	std::vector<StrayPathVariant> variants;
	int i = 0;
	for (auto& child: node) {
		variants.push_back(parseStrayPathVariant(child, tileDatas, "Variant_" + std::to_string(i++)));
	}
	return variants;
}

static std::vector<EGameDifficulty> parseDifficultyRange(const Json::Value& node) {
	if (isArray(node["difficulties"])) {
		const auto ds = algo::map_if_as<std::unordered_set<EGameDifficulty>>(node["difficulties"],
			RETLAMBDA(static_cast<EGameDifficulty>(it.asInt())),
			RETLAMBDA(it >= difficultyquery::First && it <= difficultyquery::Last));
		return { ds.begin(), ds.end() };
	}

	const auto clamped = [](int value) {
		return static_cast<EGameDifficulty>(FMath::Clamp(value, static_cast<int>(difficultyquery::First), static_cast<int>(difficultyquery::Last)));
	};
	const auto& rangeNode = node["difficulty-range"];
	const bool isRangeArray = rangeNode.isArray() && rangeNode.size() == 2;
	return difficultyquery::inclusiveBetween(
		clamped(isRangeArray ? rangeNode[0].asInt(0) : node["min-difficulty"].asInt(0)),
		clamped(isRangeArray ? rangeNode[1].asInt(enum_cast(difficultyquery::Last)) : node["max-difficulty"].asInt(enum_cast(difficultyquery::Last))));
}


//
// Mobs
//
static MobType parseMobType(const Json::Value& node) {
	if (node.isString()) {
		return MobType{ node.asString() };
	}
	if (node.isArray() && node.size() == 2) {
		return MobType { node[1].asString(), node[0].asFloat() };
	}
	MobType mobType(node["type"].asString());
	mobType.weight = node["weight"].asFloat(1);
	mobType.min = node["min"].asInt(0);
	mobType.max = node["max"].asInt(std::numeric_limits<int>::max());
	mobType.maxFractionOfTotal = node["max%"].asFloat(1);
	mobType.allowedOnDifficulties = parseDifficultyRange(node);
	return mobType;
}

static std::vector<MobType> parseMobTypes(const Json::Value& node) {
	return algo::map_vector(node, parseMobType);
}

static MobGroup parseMobGroup(const Json::Value& node) {
	MobGroup group { parseMobTypes(node["types"]), node["id"].asString() };

	const auto& pick = node["pick-types"];

	if (pick.isNull()) {
		// Do nothing
	} else if (pick.isInt()) {
		group.typesCountInterval = { 1, pick.asInt() };
	} else if (isArray(pick)) {
		group.typesCountInterval = { pick[0].asInt(), pick[1].asInt() };
	}
	group.allowedOnDifficulties = parseDifficultyRange(node);

	return group;
}

static void parseMobGroups(const Json::Value& mobGroupsNode, MobGroups& mobGroups) {
	algo::map_to(mobGroupsNode, parseMobGroup, mobGroups);
}

//
// Objectives
//
TrueCounter objectiveTypeChecker(const io::Objective& objective) {
	return { {
			false,                   // 0
			objective.arena.IsSet(), // 1
			objective.click.IsSet(), // 2...
			objective.gauntlet.IsSet() || objective.hidden.IsSet(),
			objective.killGroup.IsSet(),
	} };
}

TOptional<ObjectiveType> objectiveType(const io::Objective& objective) {
	const auto type = objectiveTypeChecker(objective);
	return type.count() == 1 ? static_cast<ObjectiveType>(type.firstIndex()) : TOptional<ObjectiveType>{};
}

static void parseObjectiveRegionLocator(const Json::Value& node, std::vector<RegionLocator>& locators) {
	if (node.isString()) {
		locators.push_back(node.asString());
		return;
	}
	if (isArray(node)) {
		for (auto&& region : node) {
			locators.push_back(region.asString());
		}
	}
}

static TOptional<RegionLocator> parseSingleObjectiveRegionLocator(const Json::Value& node) {
	std::vector<RegionLocator> locators;
	parseObjectiveRegionLocator(node, locators);
	return locators.size() == 1 ? locators.front() : TOptional<RegionLocator>{};
}

static ObjectiveGate parseObjectiveGate(const Json::Value& parent, const std::string& key) {
	ObjectiveGate gate;

	if (isDefinedAsNull(parent, key)) {
		return { ObjectiveGate::Nothing };
	}
	const auto& node = parent[key];

	if (node.isString()) {
		gate.object = node.asCString();
	}
	if (node.isObject()) {
		if (node["tile-exits"].asBool(false)) {
			gate.type = gate.type.Get(0) | ObjectiveGate::TileExits;
		}
		if (node["gates"].asBool(false)) {
			gate.type = gate.type.Get(0) | ObjectiveGate::Gates;
		}
		if (node.isMember("regions")) {
			gate.type = gate.type.Get(0) | ObjectiveGate::Regions;
			parseObjectiveRegionLocator(node["regions"], gate.regions);
		}
		// Gate prefab
		gate.object = asFString(node["object"]);
		if (node.isMember("start-unlocked")) {
			gate.startUnlocked = node["start-unlocked"].asBool();
		}
	}
	return gate;
}

template <typename T>
static TOptional<T> containsId(const std::vector<T>& collection, const std::string& id) {
	auto it = std::find_if(begin(collection), end(collection), [lowerId = Util::toLower(id)](const auto& e) { return e.lowerId == lowerId; });
	if (it != end(collection)) {
		return *it;
	}
	return {};
}

static ObjectiveMobs parseObjectiveArenaBattleWave(const Json::Value& node, const MobGroups& mobGroups) {

	ObjectiveMobs mobs;
	// Add function (making sure we 1) don't add a group twice and 2) that the group exists)
	auto addGroup = [&](const WeightedId& id) {
		if (!containsId(mobs.groups, id.lowerId)) {
			if (auto it = containsId(mobGroups, id.lowerId)) {
				it->weight = id.weight;
				mobs.groups.push_back(it.GetValue());
			}
		}
	};
	if (node.isArray() && node.size() >= 2) {
		mobs.count = node[0].asInt();
		for (Json::ArrayIndex i = 1; i < node.size(); ++i) {
			addGroup(parseWeightedId(node[i]));
		}
	}
	else {
		mobs.count = node["count"].asInt(mobs.count);
		mobs.showSpawnIndicator = node["show-spawn-indicator"].asBool(true);
		if (node.isMember("spawn-at-region")) {
			mobs.spawnAtRegion = node["spawn-at-region"].asString();
		}
		for (auto&& group : node["groups"]) {
			addGroup(parseWeightedId(group));
		}
	}
	return mobs;
}

static ObjectiveGauntletData parseObjectiveGauntletData(const Json::Value& node) {
	ObjectiveGauntletData data;
	data.endRegion = node["end-region"].asString();
	if (node.isMember("end-marker") && node["end-marker"].asBool()) {
		data.markerRegion = data.endRegion;
	}
	if (node.isMember("marker-region")) {
		data.markerRegion = node["marker-region"].asString();
	}
	data.gate = parseObjectiveGate(node, "gate");
	return data;
}

static ObjectiveArenaBattleData parseObjectiveArenaBattleData(const Json::Value& node, const MobGroups& mobGroups) {
	ObjectiveArenaBattleData data;
	data.startTime = node.isMember("start-time") ? node["start-time"].asFloat() : 0.0f;
	data.calculateTimeFromWaveStart = node["wave-start"].asBool(true);
	data.hasMusic = node["has-music"].asBool(true);
	data.waveInterval = node["interval"].asFloat(data.calculateTimeFromWaveStart? 15 : 3); // NOTE: is this intended to override the json values"?
	data.restingTime = node["rest-interval"].asFloat(3);
	data.endDelaySeconds = parseFloat(node["end-delay"]);
	data.preSpawnMobDuration = node["prespawn-mob-duration"].asFloat(5);
	data.battleMaxDuration = node["battle-max-duration"].asFloat(0);
	data.stretch = node["stretch"].asString("no-stretch");
	data.gate = parseObjectiveGate(node, "gate");
	data.spawnEffectsPack = node["spawn-effects-pack"].asString("normal");
	data.isBoss = node["is-boss"].asBool(false);
	std::string mob = node["prespawn-mob"].asString("/Game/Actors/Characters/Enemies/Illagers/ArchIllager/BP_ArchIllager_ArenaBattle");
	if (mob != "") {
		data.preSpawnMobObject = mob;
	}

	for (auto&& wave : node["waves"]) {
		data.waves.push_back(parseObjectiveArenaBattleWave(wave, mobGroups));
	}

	if (node.isMember("mob-activations")) {
		data.mobActivations = algo::map_vector(node["mob-activations"], RETLAMBDA(EntityTypeFromString(it.asString())));;
	}

	return data;
}

static ObjectiveClickData parseObjectiveClickData(const Json::Value& node, const MobGroups& mobGroups) {
	ObjectiveClickData data;
	data.count = node["count"].asInt(1);
	data.object = node["object"].asString();
	data.dummyObject = asFString(node["dummy-object"]);
	//data.rotations = parseRotation(node["rotations"]);

	for (auto&& locator : node["locations"]) {
		parseObjectiveRegionLocator(locator, data.locations);
	}
	for (auto&& locator : node["locked-doors"]) {
		parseObjectiveRegionLocator(locator, data.lockedDoors);
	}
	for (auto&& locator : node["key-locations"]) {
		parseObjectiveRegionLocator(locator, data.keyLocations);
	}
	
	if (node.isMember("key-type")) {
		data.keyType = node["key-type"].asString();
	}
	if (node.isMember("mobs")) {
		data.mobs = parseObjectiveArenaBattleWave(node["mobs"], mobGroups);
	}
	data.isSequential = node["sequential"].asBool(data.isSequential);

	if (node.isMember("spawn-regions")) {
		data.spawnRegions = std::vector<RegionLocator>{};
		parseObjectiveRegionLocator(node["spawn-regions"], data.spawnRegions.GetValue());
	}
	if (node.isMember("mob-stretch")) {
		data.mobStretch = node["mob-stretch"].asString();
	}
	data.doorPath = asFString(node["door-path"]);

	return data;
}

static ObjectiveKillGroupData parseObjectiveKillGroupData (const Json::Value& node, const MobGroups& mobGroups) {
	ObjectiveKillGroupData data;
	data.mobs = parseObjectiveArenaBattleWave(node["mobs"], mobGroups);
	data.stretch = node["stretch"].asString();
	data.showCount = node["showcount"].asBool();

	data.gate = parseObjectiveGate(node, "gate");
	parseObjectiveRegionLocator(node["spawn-regions"], data.spawnRegions);

	if (node["reward"].isArray() && node["reward"].size() == 2) {
		//rewardNode = Json::Value();
		data.amount = node["reward"][0].asInt();
		data.rewardId = node["reward"][1].asString();
	} else {
		data.rewardId = node["reward"].asString();
		data.amount = 1;
	}
	if (node.isMember("marker-region")) {
		data.markerRegion = node["marker-region"].asString();
	}

	return data;
}

static TOptional<ObjectiveSoundData> parseObjectiveSoundData(const Json::Value& node, std::string defaultSound = "") {
	if (!node.isNull())
		return ObjectiveSoundData{ node["object"].asString(defaultSound) };
	if (!defaultSound.empty())
		return ObjectiveSoundData{ defaultSound };
	
	return {};
}

static TOptional<ObjectiveConsiderTilesType> parseObjectiveConsiderTilesType(const Json::Value& node) {
	const auto s = Util::toLower(node.asString("default"));

	if (s == "all")				{ return ObjectiveConsiderTilesType::All; }
	if (s == "main")			{ return ObjectiveConsiderTilesType::Main; }
	if (s == "alwaysvisible")	{ return ObjectiveConsiderTilesType::AlwaysVisible; }
	if (s == "tile")			{ return ObjectiveConsiderTilesType::Tile; }
	if (s == "stretch")			{ return ObjectiveConsiderTilesType::Stretch; }
	return {};
}

static ObjectiveInfo parseObjectiveHeader(const Json::Value& node, const std::string& loctableId) {
	ObjectiveInfo info;
	FString locTableString = FString(loctableId.c_str());
	FName tableId = FName(*locTableString.ToLower());

	info.rawName = asFString(node["name"]).Get("");
	info.name = FText::FromStringTable(tableId, info.rawName);
	info.rawDescription = asFString(node["description"]).Get("");
	info.description = FText::FromStringTable(tableId, info.rawDescription);

	if (node.isMember("displayMode")) {
		info.displayMode = asFString(node["displayMode"]).GetValue();
	}
	info.id = asFString(node["id"]);
	info.icon = asFString(node["icon"]);
	info.isOptional = node["optional"].asBool(false);
	info.meta = asStringArray(node["meta"]).Get({});
	info.panToObjectiveStart = node["panToObjectiveStart"].asBool(true);
	info.panToObjectiveEnd = node["panToObjectiveEnd"].asBool(true);
	info.panToDuration = node["panToDuration"].asFloat(2.0f);
	info.forceObjectiveIndicator = node["objective-indicator-force"].asBool(false);
	info.considerTiles = parseObjectiveConsiderTilesType(node["consider-tiles"]);
	parseObjectiveRegionLocator(node["required-regions"], info.requiredRegions);
	
	return info;
}

static ObjectiveSoundInfo parseObjectiveSounds(const Json::Value& parentNode) {
	const Json::Value& node = parentNode["sound"];

	if (node.isNull() && parentNode.isMember("arena")) {
		return {true};
	}

	if (node.isNull()) {
		return {false};
	}

	return {
		true
	   ,parseObjectiveSoundData(node["track"])
	   ,parseObjectiveSoundData(node["completed-track"])
	   ,parseObjectiveSoundData(node["narrator"])
	};
}

static Objective parseObjective(const Json::Value& objectiveNode, const MobGroups& mobGroups, const std::string& loctableId) {
	Objective objective{
		parseObjectiveHeader(objectiveNode, loctableId),
		parseObjectiveSounds(objectiveNode)
	};

	if (objectiveNode.isMember("gauntlet")) {
		objective.gauntlet = parseObjectiveGauntletData(objectiveNode["gauntlet"]);
	}
	if (objectiveNode.isMember("hidden")) {
		objective.hidden = parseObjectiveGauntletData(objectiveNode["hidden"]);
		objective.info.isHidden = true;
	}
	if (objectiveNode.isMember("arena")) {
		objective.arena = parseObjectiveArenaBattleData(objectiveNode["arena"], mobGroups);
		objective.info.alwaysShow = true;
	}
	if (objectiveNode.isMember("click")) {
		objective.click = parseObjectiveClickData(objectiveNode["click"], mobGroups);
	}
	if (objectiveNode.isMember("killgroup")) {
		objective.killGroup = parseObjectiveKillGroupData(objectiveNode["killgroup"], mobGroups);
	}
	objective.type = objectiveType(objective).Get(ObjectiveType::None);
	objective.info.type = objective.type;

	return objective;
}

static void parseObjectives(const Json::Value& node, std::vector<Objective>& objectives, const MobGroups& mobGroups, const std::string& loctableId) {
	for (const auto& objectiveNode : node) {
		objectives.push_back(parseObjective(objectiveNode, mobGroups, loctableId));
	}
	for (size_t i = 0, count = objectives.size(); i < count; ++i) {
		objectives[i].info.objectiveIndex = i;
		objectives[i].info.objectiveCount = count;
	}
}

static ObjectiveReward parseReward(const Json::Value& node) {
	if (node.isNull()) {
		return {};
	}
	return {
		parseSingleObjectiveRegionLocator(node["region"]),
		asFString(node["object"])
	};
}

static Challenge parseChallenge(const Json::Value& node, const MobGroups& mobGroups,const std::string& loctableId) {
	std::vector<RegionLocator> triggerLocators;
	parseObjectiveRegionLocator(node["trigger"], triggerLocators);

	return {
		parseObjective(node, mobGroups, loctableId),
		parseReward(node["reward"]),
		triggerLocators.empty()? "*.*.*" : triggerLocators.front(),
		asFString(node["trigger-object"])
	};
}

static void parseChallenges(const Json::Value& node, std::vector<Challenge>& challenges, const MobGroups& mobGroups, const std::string& loctableId, const std::vector<Challenge>& challengeRegistry, std::vector<std::string>* outMissingReferencedIds) {
	for (const auto& challengeNode : node) {
		if (challengeNode.isString()) {
			const auto id = challengeNode.asString();
			// no need to parse name/label data here?

			const TOptional<FString> comparisonId(id.c_str());
			const auto it = std::find_if(challengeRegistry.begin(), challengeRegistry.end(), RETLAMBDA(it.objective.info.id == comparisonId));
			if (it != challengeRegistry.end()) {
				challenges.push_back(*it);
			} else if (outMissingReferencedIds) {
				outMissingReferencedIds->push_back(id);
			}
		} else {
			challenges.push_back(parseChallenge(challengeNode, mobGroups, loctableId));
		}
	}
}


//
// Overrides
//
static FString parseStringOverride(const Json::Value& node) {
	return node.isString() ? node.asCString() : "";
}
static Overrides parseOverrides(const Json::Value& node, const PrefabDatas& tileDatas) {
	Overrides o;
	o.propDensity = parseFloat(node["prop-density"]);
	o.ambience = asFString(node["ambience"]);
	o.audioAmbience = asFString(node["audio-ambience"]);

	const auto strayPathNode = node["side-paths"];
	o.strayPath.probability = parseFloat(strayPathNode["probability"]);
	o.strayPath.variants = parseStrayPathVariants(strayPathNode["variants"], tileDatas);

	if (strayPathNode.isMember("default")) {
		o.strayPath.fallback = parseStrayPathVariant(strayPathNode["default"], tileDatas, "Variant_default");
	}
	return o;
}

static void parseStretchTiles(const Json::Value& tileGroupsNode, std::vector<WeightedTileId>& tiles, const TileGroups& tileGroups) {
	float weightScale = 1.0f / std::max((float)tileGroupsNode.size(), 1.0f);

	for (const Json::Value& node : tileGroupsNode) {
		WeightedId group = parseWeightedId(node);
		_addTiles(tiles, group.lowerId, tileGroups, group.weight * weightScale);
	}
}

static void parseStretchMobs(const Json::Value& mobsNode, StretchMobs& mobs, const MobGroups& mobGroups, const StretchMobs& defaultMobs) {
	mobs.density = mobsNode["density"].asFloat(defaultMobs.density);

	std::vector<WeightedId> excludedGroups;
	for (const Json::Value& node : mobsNode["not"]) {
		auto name = node.asString();
		if (containsId(mobGroups, name)) {
			excludedGroups.push_back(parseWeightedId(node)); // I'm lazy...
		} else {
			mobs.excludePredicates.push_back(name); // Assume it's a predicate
		}
	}
	// Add function (making sure we 1) don't add a group twice, 2) that the group is excluded and 3) that the group exists)
	auto addGroup = [&](const WeightedId& id) {
		if (!containsId(mobs.groups, id.lowerId) && !containsId(excludedGroups, id.lowerId)) {
			if (auto it = containsId(mobGroups, id.lowerId)) {
				it->weight = id.weight;
				mobs.groups.push_back(it.GetValue());
			}
		}
	};
	for (const Json::Value& node : mobsNode["and"]) {
		addGroup(parseWeightedId(node));
	}
	for (const Json::Value& node : mobsNode["only"]) {
		addGroup(parseWeightedId(node));
	}
	if (!mobsNode.isMember("only")) {
		for (auto&& defaultGroup : defaultMobs.groups) {
			addGroup(WeightedId{defaultGroup.id, defaultGroup.weight}); // @challenges MobGroup: public WeightedId
		}
		for (auto&& excludePredicate : defaultMobs.excludePredicates) {
			algo::add_unique(mobs.excludePredicates, excludePredicate);
		}
	}
}

static void parseStretchPropGroups(const Json::Value& groupsNode, std::vector<PropId>& props, const PrefabDatas& prefabDatas) {
	float weightScale = 1.0f / std::max((float)groupsNode.size(), 1.0f);

	for (const Json::Value& node : groupsNode) {
		WeightedId group = parseWeightedId(node);
		_addProps(props, group.lowerId, prefabDatas.propGroupDefs, group.weight * weightScale);
	}
}

static TOptional<std::vector<WeightedTileId>> parseTiles(const Json::Value& node, const PrefabDatas& tileDatas) {
	std::vector<WeightedTileId> out;
	parseStretchTiles(node["tile-groups"], out, tileDatas.tileGroups);
	parseTiles(node["tiles"], out);
	return out.empty() ? TOptional<std::vector<WeightedTileId>>{} : out;
}

static void parseStretches(const Json::Value& stretchesNode, std::vector<Stretch>& stretches, const PrefabDatas& prefabDatas, const MobGroups& mobGroups, const StretchMobs& defaultMobs, const Overrides& overrides) {
	TOptional<FString> ambience;
	TOptional<FString> audioAmbience;

	for (size_t i = 0; i < stretchesNode.size(); ++i) {
		const Json::Value& node = stretchesNode[static_cast<Json::ArrayIndex>(i)];

		const auto length = node["length"].asInt(1);
		if (length <= 0) { // @todo: This is 'correct', but if it's to be removed, we need to fix tiles getting wrong stretch if length is 0
			continue;
		}

		Stretch s;
		s.id = node["id"].asString();
		s.indexInFile = i;
		s.length = length;

		auto nodeOverrides = parseOverrides(node, prefabDatas);

		ambience = algo::opt::firstValid(asFString(node["push-ambience"]), ambience);
		audioAmbience = algo::opt::firstValid(asFString(node["push-audio-ambience"]), audioAmbience);

		nodeOverrides.ambience = algo::opt::firstValid(nodeOverrides.ambience, ambience);
		nodeOverrides.audioAmbience = algo::opt::firstValid(nodeOverrides.audioAmbience, audioAmbience);

		s.overrides = merge(overrides, nodeOverrides);
		s.narration = node["narration"].asString();
		s.tiles = parseTiles(node, prefabDatas).GetValue();
		if (node.isMember("dead-ends")) {
			s.deadEnds.Emplace();
			parseTiles(node["dead-ends"], s.deadEnds.GetValue());
		}
		parseStretchMobs(node["mobs"], s.mobs, mobGroups, defaultMobs);
		parseStretchPropGroups(node["prop-groups"], s.propGroups, prefabDatas);
		s.allowRaidCaptains = node["raid-captains"].asBool(true);
		stretches.push_back(s);
	}
}

TOptional<postprocess::Config> parseFillConfig(const Json::Value& parentNode) {
	if (!parentNode.isMember("fill")) {
		return {};
	}
	const auto& fillNode = parentNode["fill"];
	if (fillNode.isNull()) {
		return postprocess::Config({});
	}

	// Handles "fill": string
	if (fillNode.isString()) {
		const auto& fillString = Util::toLower(fillNode.asString());
		if (fillString == "overworld")  { return postprocess::configs::Overworld(); }
		if (fillString == "new-cave")   { return postprocess::configs::NewCave(); }
		if (fillString == "only-doors") { return postprocess::configs::OnlyDoors(); }
		ensureMsgf(false, TEXT("Unknown fill: %s (Did you mean to use \"world\" or \"door\" nodes?"), *stringutil::toFString(fillString));
	}

	// Handles world fill in "fill": { "world": ...}
	postprocess::worldfill::BlockProvider worldProvider;
	const auto& worldNode = fillNode["world"];
	const auto worldString = worldNode.isString()? worldNode.asString() : "";

	if (auto gradient = asIntArray(worldNode["gradient"])) {
		worldProvider = postprocess::worldfill::providers::FillGradient(gradient.GetValue());
	}
	else if (worldString == "overworld") { worldProvider = postprocess::worldfill::providers::Overworld(); }
	else if (worldString == "new-cave")  { worldProvider = postprocess::worldfill::providers::NewCave(); }
	else if (worldString == "none" || isDefinedAsNull(fillNode, "world")) { /* no fill */ }
	else {
		ensureMsgf(false, TEXT("Unknown/unimplemented world-fill: %s"), *stringutil::toFString(worldString));
	}

	// Handles the door fill, and might return the resulting config
	const auto& doorNode = fillNode["door"];
	const auto doorString = doorNode.isString()? doorNode.asString() : "";
	if (doorString == "world")  { return postprocess::configs::FromWorldFillProvider(worldProvider); }
	if (doorString == "random") { return postprocess::configs::RandomDoorFrameBlock(worldProvider); }

	return postprocess::configs::FromWorldFillProvider(worldProvider);
}

static void parseDungeon(const Json::Value& node, std::vector<Dungeon>& dungeons, const PrefabDatas& tileDatas, const io::Level& level, const StretchMobs& defaultMobs, const Overrides& overrides, const std::string& defaultResourcePack) {
	Dungeon dungeon;
	dungeon.id = node["id"].asString();
	dungeon.fillConfig = parseFillConfig(node);

	dungeon.resourcePack = parseFirstResourcePack(node["resource-packs"]).Get(defaultResourcePack);
	dungeon.deathOutsideTile = node["death-outside"].asBool(true);

	if (node.isMember("dimension")) {
		dungeon.dimension = node["dimension"].asString();
	}
	dungeon.musicLevelOverride = asFString(node["music-override"]);

	auto currentDungeonOverride = overrides;
	currentDungeonOverride.ambience = algo::opt::firstValid(asFString(node["ambience"]), overrides.ambience);
	currentDungeonOverride.audioAmbience = algo::opt::firstValid(asFString(node["audio-ambience"]), overrides.audioAmbience);
	parseStretches(node["stretches"], dungeon.stretches, tileDatas, level.mobGroups, defaultMobs, currentDungeonOverride);

	for (auto& stretch : dungeon.stretches) {
		stretch.dungeonIndex = dungeons.size();
	}
	const auto outerDefinitionLevelId = stringutil::toFString(level.id);

	dungeon.level = DungeonLevelInfo {
		outerDefinitionLevelId,
		asFString(node["ambience-level-id"]).Get(outerDefinitionLevelId),
		algo::map_vector(level.objectGroups, RETLAMBDA(it.name.lowerId))
	};
	dungeons.push_back(dungeon);
}

static void parseDungeons(const Json::Value& dungeonsNode, std::vector<Dungeon>& dungeons, const PrefabDatas& tileDatas, const io::Level& level, const StretchMobs& defaultMobs, const Overrides& overrides, const std::string& defaultResourcePack) {
	for (const Json::Value& node : dungeonsNode) {
		parseDungeon(node, dungeons, tileDatas, level, defaultMobs, overrides, defaultResourcePack);
	}
}

TOptional<Json::Value> parseJson(std::istream& in, const std::string& id = "") {
	Json::Value root;
	Json::Reader reader;

	if (reader.parse(in, root)) {
		return root;
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to parse %s json: %s\n"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(reader.getFormattedErrorMessages().c_str()));
		return {};
	}
}

static io::LevelGenConfig parseLevelGenConfig(const Json::Value& node) {
	return {
		node["require-matching-doors"].asBool(false)
	};
}

static void parseLevelHeader(const Json::Value& node, LevelHeader& header) {
	header.id = node["id"].asString();
	header.loctableId = node["loctable-id"].asString(header.id);
	header.levelGenConfig = parseLevelGenConfig(node);

	for (const Json::Value& group : node["object-groups"]) {
		header.objectGroups.push_back(ObjectGroupDef(group.asString()));
	}
	header.mobGroupFileIds = asStringArray<CaseInsensitiveId>(node["mob-files"]).Get({});
}

TOptional<LevelHeader> loadLevelHeader(std::istream& in) {
	if (auto root = parseJson(in, "level-header")) {
		LevelHeader header;
		parseLevelHeader(root.GetValue(), header);
		return header;
	}
	return {};
}

void parseLevel(Level& level, const Json::Value& node) {
	parseLevelHeader(node, level);

	parseMobGroups(node["mob-groups"], level.mobGroups);
	std::vector<Challenge> toplevelChallenges;
	parseChallenges(node["challenges"], toplevelChallenges, level.mobGroups, level.loctableId, {});

	parseTileDefs(node["props"], level.propDefs, level.mobGroups, level.id, toplevelChallenges);
	parseTileDefs(node["tiles"], level.tileDefs, level.mobGroups, level.id, toplevelChallenges);

	TileGroups tileGroups;
	parseTileGroupDefs(node["tile-groups"], tileGroups);

	PropGroupDefs propGroupDefs;
	parsePropGroupDefs(node["prop-groups"], propGroupDefs);

	PrefabDatas prefabDatas{ tileGroups, propGroupDefs };

	if (node.isMember("dimension")) {
		level.dimension = node["dimension"].asString();
	}

	Overrides levelOverrides = parseOverrides(node, prefabDatas);
	levelOverrides.ambience = levelOverrides.ambience.Get("default");

	level.passThrough.definedLevelIds.push_back(level.id);
	level.passThrough.objectGroups = level.objectGroups;
	level.passThrough.playIntro = node["play-intro"].asBool(level.passThrough.playIntro);
	level.passThrough.characterLoadout = asUEnumValue<ECharacterLoadoutType>(node["character-loadout"]).Get(level.passThrough.characterLoadout);
	level.passThrough.mobGroups = level.mobGroups;

	parseObjectives(node["objectives"], level.passThrough.objectives, level.mobGroups, level.loctableId);

	StretchMobs defaultMobs;
	parseStretchMobs(node["default-mobs"], defaultMobs, level.mobGroups, StretchMobs());

	// We parse the level json object (containing the main path) as the first dungeon
	const auto defaultResourcePack = parseFirstResourcePack(node["resource-packs"]).Get("");
	parseDungeon(node, level.dungeons, prefabDatas, level, defaultMobs, levelOverrides, defaultResourcePack);

	Overrides dungeonsOverrides = levelOverrides;
	dungeonsOverrides.strayPath = {0};

	parseDungeons(node["dungeons"], level.dungeons, prefabDatas, level, defaultMobs, dungeonsOverrides, defaultResourcePack);

	level.initialDungeons = parseWeightedIds(node["initial-dungeons"]);
	if (level.initialDungeons.empty()) {
		level.initialDungeons.push_back(level.dungeons.front().id);
	}

	for (int i = 0; i < level.dungeons.size(); ++i) {
		auto& dungeon = level.dungeons[i];
		dungeon.index = i; // Add a unique index to all dungeons (names could theoretically be the same)
		dungeon.dimension = algo::opt::firstValid(dungeon.dimension, level.dimension);
		dungeon.needsToBeValidated = algo::any_of(level.initialDungeons, RETLAMBDA(it == dungeon.id));
	}

	algo::map_cast_to(level.dungeons, level.passThrough.dungeons);
}

bool loadLevel(Level& level, std::istream& in) {
	// make sure to always load string tables before parsing level json!
	missions::ensureLoadAllStringTables();

	if (auto root = parseJson(in, "level")) {
		parseLevel(level, root.GetValue());
		return true;
	}
	return false;
}

Unique<Level> loadLevel(std::istream& in) {
	auto level = std::make_unique<Level>();
	if (loadLevel(*level, in)) {
		return level;
	}
	return nullptr;
}

bool loadObjectives(Level& level, std::istream& in) {
	if (!level.passThrough.objectives.empty()) {
		std::cout << "We already had objectives when loading new ones!" << std::endl;
		return false;
	}
	if (auto root = parseJson(in, "objectives")) {
		parseObjectives(root.GetValue(), level.passThrough.objectives, level.mobGroups, level.id);
		return true;
	}
	return false;
}

bool loadMobGroups(Level& level, std::istream& in) {
	if (auto root = parseJson(in, "mobGroups")) {
		parseMobGroups(root.GetValue(), level.mobGroups);
		return true;
	}
	return false;
}

TOptional<ItemArchetypeCounts> parseItemArchetypeCounts(const Json::Value& node) {
	std::unordered_map<EItemArchetype, int> counts;
	for (auto& name : node.getMemberNames()) {
		if (auto key = GetEnumValueFromStringT<EItemArchetype>(stringutil::toFString(name))) {
			counts[key.GetValue()] = node[name].asInt();
		}
		else {
			return {};
		}
	}
	return ItemArchetypeCounts(counts);
}

TOptional<HyperDungeon> parseHyperDungeon(const Json::Value& node) {
	const auto id = parseWeightedId(node);
	if (id.lowerId != "") {
		return HyperDungeon {
			id,
			json::common::stringEnum<EDLCName>(node["required-dlc"].asString("")),
			parseItemArchetypeCounts(node["archetype-requirements"]).Get({}),
			parseArrayMaybeT<EntityType>(node["mob-types"], RETLAMBDA(MaybeEntityTypeFromString(it.asString()))),
			node["archetype-requirements"].isNull()
		};
	}
	return {};
}

std::unordered_map<ELevelNames, HyperMobGroup> parseHyperLevelMobs(const Json::Value& node) {
	const auto getMobTypeForId = [types = parseMobTypes(node["hypermission-mobs"])](const Json::Value& value) {
		const auto type = Util::toLower(value.asString());
		const auto i = algo::index_of_if(types, RETLAMBDA(Util::toLower(it.expr) == type));
		return i ? types[i.GetValue()] : TOptional<io::MobType>{};
	};

	std::unordered_map<ELevelNames, HyperMobGroup> out;

	for (const auto& mappingNode : node["mission-mapping"]) {
		const HyperMobGroup hyperMobGroup = {
			parseArrayMaybeT<io::MobType>(mappingNode["types"], getMobTypeForId)
		};
		for (const auto levelId : parseEnumArrayT<ELevelNames>(mappingNode["ids"])) {
			out[levelId] = hyperMobGroup;
		}
	}
	return out;
}

void parseHyperLevel(io::HyperLevel& level, const Json::Value& node) {
	level.levelIds = parseWeightedIds(node["levels"]);
	level.definitionLevelIds = parseWeightedIds(node["definition-levels"]);
	level.hyperDungeons = parseArrayMaybeT<HyperDungeon>(node["hyperdungeons"], parseHyperDungeon);
	level.hyperLevelMobs = parseHyperLevelMobs(node);
}

Unique<HyperLevel> loadHyperLevel(std::istream& in) {
	auto level = std::make_unique<HyperLevel>();

	if (auto root = parseJson(in, "hyperlevel")) {
		parseHyperLevel(*level, root.GetValue());
		return level;
	}
	return {};
}

}
