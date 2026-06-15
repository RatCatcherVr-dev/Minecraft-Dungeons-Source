#include "CharacterSaveData.h"
#include "JsonCommon.h"
#include "BinarySaveData.h"
#include "util/StringUtil.h"
#include "util/Algo.h"
#include "util/EnumUtil.h"
#include "util/SharedRandom.h"
#include "Kismet/KismetGuidLibrary.h"
#include "world/entity/EntityTypes.h"
#include "game/skins/SkinsUtil.h"
#include "game/merchant/MerchantDefs.h"
#include "game/progress/ProgressStat.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "writer.h"
#include "PlatformFilemanager.h"
#include "FileManager.h"
#include "DungeonsGameInstance.h" // D11.SSN
#include "Kismet/KismetInternationalizationLibrary.h" // D11.SSN
#include "DungeonsSaveVersion.h"
#include "online/crossplay/UserCloud.h"
#include "game/difficulty/endless/EndlessStruggleTiers.h"
#include "game/component/MissionProgressComponent.h"

#define LOCTEXT_NAMESPACE "Save_Data_Character"

namespace json { namespace character {



Json::Value toJson(const EnchantmentSaveData& enchantment) {
	Json::Value node;
	node["id"] = json::common::enumString(enchantment.id);
	node["level"] = enchantment.level;
	return node;
}

Json::Value toJson(const ArmorPropertySaveData& armorProperty) {
	Json::Value node;
	node["id"] = json::common::enumString(armorProperty.id);
	node["rarity"] = json::common::enumString(armorProperty.rarity);
	return node;
}

Json::Value toJson(const FItemId& id) {
	return stringutil::toStdString(id.GetBackingType());
}

Json::Value toJson(const ItemSaveData& item) {
	Json::Value node;
	node["type"] = toJson(item.type);
	node["power"] = item.power;
	node["rarity"] = json::common::enumString(item.rarity);
	node["upgraded"] = item.upgraded;

	if (item.markedNew) {
		//No need to populate json with a billion 'false' since that will be the eventual state.
		node["markedNew"] = true;
	}

	if (item.gifted) {
		//No need to populate json with a billion 'false' since that is the most common case.
		node["gifted"] = true;
	}
	
	if (item.modified) {
		//No need to populate json with a billion 'false' since that is the most common case.
		node["modified"] = true;
	}

	if (item.cloned) {
		//Only save value in the file if we have a known value,
		node["cloned"] = item.cloned;
	}

	if (item.equipped) {
		node["equipmentSlot"] = json::common::enumString(item.GetEquipmentSlot());
	}
	else {
		node["inventoryIndex"] = item.GetIndex();
	}
	if (!item.enchantments.empty()) {
		Json::Value enchantmentNodes(Json::arrayValue);
		for (const auto& enchantment : item.enchantments) {
			enchantmentNodes.append(toJson(enchantment));
		}
		node["enchantments"] = enchantmentNodes;
	}
	if (!item.armorProperties.empty()) {
		Json::Value armorPropertyNodes(Json::arrayValue);
		for (const auto& armorProperty : item.armorProperties) {
			armorPropertyNodes.append(toJson(armorProperty));
		}
		node["armorproperties"] = armorPropertyNodes;
	}
	if (item.netheriteEnchant) {
		node["netheriteEnchant"] = toJson(item.netheriteEnchant.GetValue());
	}

	return node;
}

Json::Value toJson(const CosmeticsSaveData& cosmetic) {
	if (cosmetic.type == ECosmeticType::Unset || cosmetic.type == ECosmeticType::Invalid) {
		return Json::nullValue;
	}

	Json::Value node;
	node["id"] = stringutil::toStdString(cosmetic.id.ToString());
	node["type"] = json::common::enumString(cosmetic.type);
	return node;
}

Json::Value toJson(const UIHintProgressSaveData& uiHint) {
	Json::Value node;
	node["hintType"] = json::common::enumString(uiHint.hintType);
	return node;
}

Json::Value toJson(const CurrencySaveData& currency) {
	Json::Value node;
	node["type"] = toJson(currency.type);
	node["count"] = currency.count;
	return node;
}

Json::Value toJson(const MissionProgressSaveData& progress) {
	Json::Value node;
	if (progress.completedDifficulty.IsSet()) {
		node["completedDifficulty"] = json::common::enumString(progress.completedDifficulty.Get(EGameDifficulty::Invalid));
	}
	if (progress.completedThreatLevel.IsSet()) {
		node["completedThreatLevel"] = json::common::enumString(progress.completedThreatLevel.Get(EThreatLevel::Invalid));
	}
	node["completedEndlessStruggle"] = progress.completedEndlessStruggle.Value;
	return node;
}

Json::Value toJson(const StrongholdProgressSaveData& stronghold) {
	Json::Value node;

	for (auto& progress : stronghold.progresses) {
		if (const auto name = eyeofenderquery::toString(progress.Key)) {
			node["stronghold" + name.GetValue() + "Unlocked"] = progress.Value.unlocked;
			node["strongholdHas" + name.GetValue() + "Eye"] = progress.Value.hasEye;
		} else {
			checkNoEntry();
		}
	}
	node["strongholdUsedPortal"] = stronghold.usedPortal;
	return node;
}

Json::Value toJson(const ChestSaveData& chestProgress) {
	Json::Value node;
	node["unlockedTimes"] = chestProgress.unlockedTimes;
	return node;
}

Json::Value toJson(const DifficultyProgressSaveData& difficulties) {
	Json::Value node;

	if (difficulties.unlocked.IsSet()) {
		node["unlocked"] = json::common::enumString(difficulties.unlocked.Get(EGameDifficulty::Invalid));
	}

	if (difficulties.announced.IsSet()) {
		node["announced"] = json::common::enumString(difficulties.announced.Get(EGameDifficulty::Invalid));
	}

	if (difficulties.selected.IsSet()) {
		node["selected"] = json::common::enumString(difficulties.selected.Get(EGameDifficulty::Invalid));
	}

	return node;
}

Json::Value toJson(const ThreatLevelProgressSaveData& threatLevels) {
	Json::Value node;

	if (threatLevels.unlocked.IsSet()) {
		node["unlocked"] = json::common::enumString(threatLevels.unlocked.Get(EThreatLevel::Invalid));
	}

	if (threatLevels.announced.IsSet()) {
		node["announced"] = json::common::enumString(threatLevels.announced.Get(EThreatLevel::Invalid));
	}

	return node;
}

Json::Value toJson(const EndGameContentProgressSaveData& data) {
	using namespace json::common;

	Json::Value node;
	node["announcedUnlockedContent"] = iterableToJsonArray(data.announcedUnlockedContent, RETLAMBDA(enumString(it)));
	return node;
}

Json::Value toJson(const FVector2D& vec) {
	Json::Value node;

	node["x"] = std::to_string(vec.X);
	node["y"] = std::to_string(vec.Y);

	return node;
}

Json::Value toJson(const MapUIState& mapUIState) {
	using namespace json::common;
	Json::Value node;

	if (mapUIState.selectedRealm.IsSet()) {
		node["selectedRealm"] = enumString(mapUIState.selectedRealm.Get(ERealmName::Invalid));
	}

	if (mapUIState.selectedDifficulty.IsSet()) {
		node["selectedDifficulty"] = enumString(mapUIState.selectedDifficulty.Get(EGameDifficulty::Invalid));
	}

	if (mapUIState.selectedThreatLevel.IsSet()) {
		node["selectedThreatLevel"] = enumString(mapUIState.selectedThreatLevel.Get(EThreatLevel::Invalid));
	}

	if (mapUIState.selectedMission.IsSet()) {
		node["selectedMission"] = enumString(mapUIState.selectedMission.Get(ELevelNames::Invalid));
	}

	if (mapUIState.selectedThreatLevel.IsSet()) {
		node["selectedThreatLevel"] = enumString(mapUIState.selectedThreatLevel.Get(EThreatLevel::Invalid));
	}

	if (mapUIState.panPosition.IsSet()) {
		node["panPosition"] = toJson(mapUIState.panPosition.GetValue());
	}

	return node;
}

Json::Value toJson(const UCharacterSaveData::MissionProgressMap& progress) {
	Json::Value node;
	for (const auto& missionEntry : progress) {
		node[json::common::enumString(missionEntry.first)] = toJson(missionEntry.second);
	}
	return node;
}

Json::Value toJson(const std::unordered_map<int32, ChestSaveData>& chestsProgress) {
	Json::Value node;
	for (const auto& chestProgressPair : chestsProgress) {
		node[Util::toString(chestProgressPair.first)] = toJson(chestProgressPair.second);
	}
	return node;
}


Json::Value toJson(const std::unordered_map<EntityType, int32>& mobKills) {
	Json::Value node;
	for (const auto& mobKillPair : mobKills) {
		node[EntityTypeToString(mobKillPair.first)] = mobKillPair.second;
	}
	return node;
}

Json::Value toJson(const EDLCName& dlc) {
	return Json::Value(json::common::enumString(dlc));
}

Json::Value toJson(const FInventoryItemData& item) {
	const auto& type = GetItemRegistry().Get(item.GetItemId());

	Json::Value node;
	node["type"] = toJson(type.getId());
	node["power"] = item.ItemPower;
	node["rarity"] = json::common::enumString(item.Rarity);
	node["gifted"] = item.IsGifted();
	node["upgraded"] = item.IsUpgraded();
	node["modified"] = item.IsModified();

	if (item.Enchantments.Num() > 0) {
		Json::Value enchantmentNodes(Json::arrayValue);
		for (const auto& enchantment : item.Enchantments) {
			const EnchantmentSaveData &ench = { enchantment.TypeID, enchantment.Level };
			enchantmentNodes.append(toJson(ench));
		}
		node["enchantments"] = enchantmentNodes;
	}
	if (item.ArmorProperties.Num() > 0) {
		Json::Value armorPropertyNodes(Json::arrayValue);
		for (const auto& armorProperty : item.ArmorProperties) {
			const ArmorPropertySaveData &armProp = ArmorPropertySaveData(armorProperty);
			armorPropertyNodes.append(toJson(armProp));
		}
		node["armorproperties"] = armorPropertyNodes;
	}
	if (item.OverrideStoreCount) {
		node["overrideStoreCount"] = item.OverrideStoreCount.GetValue();
	}
	if (item.IsNetherite()) {
		const auto& netheriteEnchant = item.NetheriteEnchant();
		const EnchantmentSaveData &ench = { netheriteEnchant.TypeID, netheriteEnchant.Level };
		node["netheriteEnchant"] = toJson(ench);
	}

	return node;
}

Json::Value toJson(const FRewardData& reward)
{
	Json::Value node;
	node["type"] = common::enumString(reward.RewardType);
	node["itemData"] = toJson(reward.ItemData);

	return node;
}

//Merchants
Json::Value toJson(const FMerchantSlotSaveData& slotdata) {
	Json::Value node;
	if (slotdata.item.IsSet()) {
		node["item"] = toJson(slotdata.item.GetValue());;
	}
	node["priceMultiplier"] = slotdata.mPriceMultiplier;
	node["rebateFraction"] = slotdata.mRebateFraction;
	node["reserved"] = slotdata.bReserved;
	return node;
}

Json::Value toJson(const FCountQuestState& queststatedata) {
	Json::Value node;
	node["targetCount"] = queststatedata.mTargetCount;
	node["startedAtCount"] = queststatedata.mStartedAtCount;
	return node;
}

Json::Value toJson(const FDynamicQuestState& questdynamicdata) {
	Json::Value node;
	if (questdynamicdata.mSelectedProgressStat.IsSet()) {
		node["selectedProgressStat"] = json::common::enumString(questdynamicdata.mSelectedProgressStat.GetValue());
	}
	return node;
}

Json::Value toJson(const FMerchantQuestSaveData& questdata) {
	Json::Value node;
	if (questdata.mCountQuestState.IsSet()) {
		node["questState"] = toJson(questdata.mCountQuestState.GetValue());;
	}
	if (questdata.mDynamicQuestState.IsSet()) {
		node["dynamicQuestState"] = toJson(questdata.mDynamicQuestState.GetValue());;
	}
	return node;
}

Json::Value toJson(const FMerchantPricingSaveData& pricingdata) {
	Json::Value node;
	node["timesRestocked"] = pricingdata.mTimesRestocked;
	return node;
}

Json::Value toJson(const FMerchantSaveData& merchant) {
	Json::Value node;
	node["everInteracted"] = merchant.mEverInteracted;
	node["slots"] = json::common::TMapToJsonMap(merchant.mSlots, RETLAMBDA(stringutil::toStdString(it)), RETLAMBDA(toJson(it)));
	node["quests"] = json::common::TMapToJsonMap(merchant.mQuests, RETLAMBDA(stringutil::toStdString(it)), RETLAMBDA(toJson(it)));
	node["pricing"] = toJson(merchant.mPricing);
	return node;
}

Json::Value toJson(const CompletedTrialData& completedTrial) {
	Json::Value node;

	node["id"] = stringutil::toStdString(completedTrial.id);
	node["difficulty"] = stringutil::toStdString(GetEnumValueToStringStripped(completedTrial.difficulty));

	return node;
}

// MissionState
Json::Value toJson(const FMissionDifficulty& missionDifficulty) {
	Json::Value node;
	node["mission"] = json::common::enumString(missionDifficulty.mission);
	node["difficulty"] = json::common::enumString(missionDifficulty.difficulty);
	node["threatLevel"] = json::common::enumString(missionDifficulty.threatLevel);
	node["endlessStruggle"] = missionDifficulty.endlessStruggle.Value;
	return node;
}

Json::Value toJson(const FMissionState& missionState) {
	Json::Value node;
	node["missionDifficulty"] = toJson(missionState.missionDifficulty);
	node["ownedDLCs"] = json::common::iterableToJsonArray(missionState.ownedDLCs, RETLAMBDA(toJson(it)));
	node["offeredItems"] = json::common::iterableToJsonArray(missionState.offeredItems, RETLAMBDA(toJson(it)));
	node["offeredEnchantmentPoints"] = missionState.offeredEnchantmentPoints;
	node["livesLost"] = missionState.livesLost;
	node["partsDiscovered"] = missionState.partsDiscovered;
	node["guid"] = stringutil::toStdString(missionState.guid);
	node["seed"] = missionState.seed;
	node["completedOnce"] = missionState.bCompletedOnce;
	return node;
}



/// LOAD ///

Json::Value toJson(const TOptional<FInventoryItemData>& item) {
	if (item.IsSet()) {
		return toJson(item.GetValue());
	}

	return Json::nullValue;
}

EnchantmentSaveData parseEnchantment(const Json::Value& node) {
	EnchantmentSaveData enchantment;
	enchantment.id = EnumValueFromString(EEnchantmentTypeID, stringutil::toFString(node["id"].asString())).Get(EEnchantmentTypeID::Unset);
	enchantment.level = node["level"].asInt();
	return enchantment;
}

ArmorPropertySaveData parseArmorProperty(const Json::Value& node) {
	ArmorPropertySaveData armorProperty;
	armorProperty.id = EnumValueFromString(EArmorPropertyID, stringutil::toFString(node["id"].asString())).Get(EArmorPropertyID::Unset);
	if (node.isMember("rarity")) {
		armorProperty.rarity = game::item::rarity::fromString(node["rarity"].asString());
	}
	else {
		armorProperty.rarity = EItemRarity::Common;
	}
	return armorProperty;
}

TOptional<FItemId> parseItemId(const Json::Value& node) {
	return GetItemRegistry().Request(stringutil::toFName(node.asString()));
}

TOptional<ItemSaveData> parseItem(const Json::Value& node) {
	if (node.isNull()) {
		return {};
	}

	if(auto id = parseItemId(node["type"])) {
		const auto& itemType = GetItemRegistry().Get(id.GetValue());
		ItemSaveData item(id.GetValue());

		item.type = id.GetValue();
		item.power = node["power"].asFloat();
		item.rarity = game::item::rarity::fromString(node["rarity"].asString());
		item.upgraded = node["upgraded"].asBool();
		item.modified = node["modified"].asBool();

		//No need to populate json with a billion 'false' since that will be the eventual end state for all items.
		item.markedNew = node.isMember("markedNew") ? node["markedNew"].asBool() : false;
		item.gifted = node.isMember("gifted") ? node["gifted"].asBool() : false;

		item.cloned = node.isMember("cloned") ? node["cloned"].asBool() : false;
		if (!item.cloned && node.isMember("value")) { // Converting old "cloned" by value == 0 information.
			item.cloned = node["value"].asInt() == 0;
		}

		item.equipmentSlot = EnumValueFromString(EEquipmentSlot, stringutil::toFString(node["equipmentSlot"].asString())).Get(EEquipmentSlot::Invalid);
		item.inventoryIndex = node["inventoryIndex"].asInt(0);
		item.equipped = item.equipmentSlot != EEquipmentSlot::Invalid;

		for (const auto& enchantment : node["enchantments"]) {
			item.enchantments.push_back(parseEnchantment(enchantment));
		}

		for (const auto& armorProperty : node["armorproperties"]) {
			item.armorProperties.push_back(parseArmorProperty(armorProperty));
		}

		item.netheriteEnchant = node.isMember("netheriteEnchant") ? parseEnchantment(node["netheriteEnchant"]) : TOptional<EnchantmentSaveData>();

		return item;
	}

	UE_LOG(LogDungeons, Warning, TEXT("Encounted nonexistant Item id %s when reading save data, dropping entry."), *stringutil::toFString(node["type"].asCString()));


	return {};
}

CosmeticsSaveData parseCosmetic(const Json::Value& node) {
	if (!node.isNull()) {
		const auto cosmeticName = node["id"].asString();
		const auto cosmeticType = game::cosmetics::fromString(node["type"].asString());
		return CosmeticsSaveData(FName(*stringutil::toFString(cosmeticName)), cosmeticType);
	}
	return {};
}

DifficultyProgressSaveData parseDifficulties(const Json::Value& node) {
	DifficultyProgressSaveData difficulties = {};
	if (node.isMember("unlocked")) {
		difficulties.unlocked = EnumValueFromString(EGameDifficulty, stringutil::toFString(node["unlocked"].asString()));
	}
	if (node.isMember("announced")) {
		difficulties.announced = EnumValueFromString(EGameDifficulty, stringutil::toFString(node["announced"].asString()));
	}
	if (node.isMember("selected")) {
		difficulties.selected = EnumValueFromString(EGameDifficulty, stringutil::toFString(node["selected"].asString()));
	}
	return difficulties;
}

ThreatLevelProgressSaveData parseThreatLevels(const Json::Value& node) {
	ThreatLevelProgressSaveData threatLevels = {};
	if (node.isMember("unlocked")) {
		threatLevels.unlocked = EnumValueFromString(EThreatLevel, stringutil::toFString(node["unlocked"].asString()));
	}
	if (node.isMember("announced")) {
		threatLevels.announced = EnumValueFromString(EThreatLevel, stringutil::toFString(node["announced"].asString()));
	}
	return threatLevels;
}

EndGameContentProgressSaveData parseEndGameContentProgress(const Json::Value& node) {
	EndGameContentProgressSaveData endgameContentProgress = {};

	for (const auto& contentId : node["announcedUnlockedContent"]) {
		if (auto contentIdEnum = EnumValueFromString(EEndGameContentType, stringutil::toFString(contentId.asString())))
			endgameContentProgress.announcedUnlockedContent.Add(contentIdEnum.GetValue());

	}

	return endgameContentProgress;
}

TMap<EProgressStat, int32> parseProgressStatCounters(const Json::Value& node) {
	TMap<EProgressStat, int32> map;
	for (const auto& counter : node.getMemberNames()) {
		auto key = EnumValueFromString(EProgressStat, stringutil::toFString(counter));
		if (key.IsSet()) {
			map.Add(key.GetValue(), node[counter].asInt());
		}
	}
	return map;
}

TSet<FString> parseVideoPlaybackCount(const Json::Value& node) {
	TSet<FString> set;
	for (const auto& counter : node.getMemberNames()) {
		auto key = stringutil::toFString(counter);
		set.Add(key);
	}
	return set;
}

FVector2D parseVector2D(const Json::Value& node) {
	if (node.isMember("x") && node.isMember("y")) {
		//D11.PS - Temp fix for this crashing, because I am not sure if we need to change where this is converted from Json or if the problem is in this function
		//rather than changing things I dont know about I changed this function to work with both values as float or string. This needs to be fixed properly at some point.
		float x = 0.0f;
		float y = 0.0f;

		auto nodeX = node["x"];
		if (nodeX.isNumeric())
		{
			x = nodeX.asFloat(0.0f);
		}
		else if (nodeX.isString())
		{
			std::string stringX = nodeX.asString();
			x = ::atof(stringX.c_str());
		}

		auto nodeY = node["y"];
		if (!nodeY.isNull() && nodeY.isNumeric())
		{
			y = nodeY.asFloat(0.0f);
		}
		else if (nodeY.isString())
		{
			std::string stringY = nodeY.asString();
			y = ::atof(stringY.c_str());
		}

		return FVector2D(x, y);
	}
	return FVector2D(0.f, 0.f);
}

MapUIState parseMapUIState(const Json::Value& node) {
	MapUIState mapUIState = {};
	if (node.isMember("selectedRealm")) {
		mapUIState.selectedRealm = EnumValueFromString(ERealmName, stringutil::toFString(node["selectedRealm"].asString()));
	}
	if (node.isMember("selectedDifficulty")) {
		mapUIState.selectedDifficulty = EnumValueFromString(EGameDifficulty, stringutil::toFString(node["selectedDifficulty"].asString()));
	}
	if (node.isMember("selectedThreatLevel")) {
		mapUIState.selectedThreatLevel = EnumValueFromString(EThreatLevel, stringutil::toFString(node["selectedThreatLevel"].asString()));
	}
	if (node.isMember("selectedMission")) {
		mapUIState.selectedMission = level::fromString(stringutil::toFString(node["selectedMission"].asString()));
	}
	if (node.isMember("panPosition")) {
		mapUIState.panPosition = parseVector2D(node["panPosition"]);
	}
	return mapUIState;
}

MissionProgressSaveData parseMissionProgress(const Json::Value& node) {
	MissionProgressSaveData mp;
	if (node.isMember("completedDifficulty")) {
		mp.completedDifficulty = EnumValueFromString(EGameDifficulty, stringutil::toFString(node["completedDifficulty"].asString())).Get(EGameDifficulty::Invalid);
	}
	if (node.isMember("completedThreatLevel")) {
		mp.completedThreatLevel = EnumValueFromString(EThreatLevel, stringutil::toFString(node["completedThreatLevel"].asString())).Get(EThreatLevel::Invalid);
	}
	if( node.isMember("completedEndlessStruggle")) {
		mp.completedEndlessStruggle.Value = node["completedEndlessStruggle"].asInt();
	}
	return mp;
}

TOptional<UIHintProgressSaveData> parseUIHintProgress(const Json::Value& node) {
	if (!node.isMember("hintType")) {
		return {};
	}

	if (const auto value = EnumValueFromString(EUIHintType, stringutil::toFString(node["hintType"].asString()))) {
		return UIHintProgressSaveData{ value.GetValue() };
	}
	else {
		return {};
	}
}

TOptional<CurrencySaveData> parseCurrencyItem(const Json::Value& node) {
	if (!node.isNull()) {
		if (auto id = parseItemId(node["type"])) {
			return CurrencySaveData(id.GetValue(), node["count"].asUInt());
		}
	}

	return {};
}

StrongholdProgressSaveData parseStrongholdProgress(const Json::Value& node) {
	StrongholdProgressSaveData strongholdProgressSaveData;

	for (const auto type : eyeofenderquery::AllEyeOfEnderTypes) {
		const auto name = eyeofenderquery::toString(type).GetValue();
		strongholdProgressSaveData.progresses.Add(type, {
			node["stronghold" + name + "Unlocked"].asBool(),
			node["strongholdHas" + name + "Eye"].asBool()
		});
	}

	strongholdProgressSaveData.usedPortal = node["strongholdUsedPortal"].asBool();

	return strongholdProgressSaveData;
}

ChestSaveData parseLobbyChestProgress(const Json::Value& node) {
	ChestSaveData chestProgress;
	chestProgress.unlockedTimes = node["unlockedTimes"].asInt();
	return chestProgress;
}

TOptional<EDLCName> parseDLC(const Json::Value& node) {
	return json::common::stringEnum<EDLCName>(node.asString(""));
}

TOptional<FInventoryItemData> parseInventoryItemData(const Json::Value& node) {
	const auto id = parseItemId(node["type"]);
	if (!id) {
		return {};
	}
	const auto& itemType = GetItemRegistry().Get(id.GetValue());
	const float itemPower = node["power"].asFloat();
	const auto rarity = game::item::rarity::fromString(node["rarity"].asString());
	const bool isUpgraded = node["upgraded"].asBool(false);
	const bool isGifted = node["gifted"].asBool(false);
	const bool isModified = node["modified"].asBool(false);

	TOptional<FEnchantmentData> netheriteEnchant;
	if (node.isMember("netheriteEnchant")) {
		const auto netherite = parseEnchantment(node["netheriteEnchant"]);
		netheriteEnchant = { netherite.id, netherite.level };
	}

	TArray<FEnchantmentData> enchantments;
	TArray<FArmorPropertyData> armorProperties;

	if (node.isMember("enchantments")) {
		for (const auto& jsonEnchantment : node["enchantments"]) {
			EnchantmentSaveData enchantment = parseEnchantment(jsonEnchantment);
			enchantments.Emplace(enchantment.id, enchantment.level);
		}
	}

	if (node.isMember("armorproperties")) {
		for (const auto& jsonArmorProperty : node["armorproperties"]) {
			ArmorPropertySaveData armorProperty = parseArmorProperty(jsonArmorProperty);
			armorProperties.Emplace(armorProperty.id, armorProperty.rarity);
		}
	}

	FInventoryItemData itemData{ itemType.getId(), itemPower, MoveTemp(enchantments), netheriteEnchant, MoveTemp(armorProperties), rarity, isUpgraded, isGifted, isModified };
	if (node.isMember("overrideStoreCount")) {
		itemData.OverrideStoreCount = node["overrideStoreCount"].asInt();
	}
	return itemData;
}

TOptional<FInventoryItemData> parsePendingRewardItem(const Json::Value& node) {
	return parseInventoryItemData(node);
}

TOptional<FRewardData> parsePendingRewardData(const Json::Value& node) {
	auto rewardType = GetEnumValueFromStringT<ERewardType>(stringutil::toFString(node["type"].asString("")));
	if (!rewardType) {
		return {};
	}
	auto rewardItem = parseInventoryItemData(node["itemData"]);
	if (!rewardItem) {
		return {};
	}
	return FRewardData(rewardType.GetValue(), rewardItem.GetValue());
}


//Merchants
FMerchantSlotSaveData parseMerchantSlotData(const Json::Value& node) {
	FMerchantSlotSaveData slotdata;
	if (node.isMember("item")) {
		slotdata.item = parseInventoryItemData(node["item"]);
	}
	slotdata.mRebateFraction = node["rebateFraction"].asFloat(0.0f);
	slotdata.mPriceMultiplier = node["priceMultiplier"].asFloat(1.0f);
	slotdata.bReserved = node["reserved"].asBool(false);
	return slotdata;
}

FCountQuestState parseMerchantCountQuestStateData(const Json::Value& node) {
	FCountQuestState queststatedata;
	queststatedata.mTargetCount = node["targetCount"].asInt(0);
	queststatedata.mStartedAtCount = node["startedAtCount"].asInt(0);
	return queststatedata;
}

FDynamicQuestState parseMerchantDynamicQuestStateData(const Json::Value& node) {
	FDynamicQuestState questdynamicdata;
	if (node.isMember("selectedProgressStat")) {
		questdynamicdata.mSelectedProgressStat = GetEnumValueFromStringT<EProgressStat>(stringutil::toFString(node["selectedProgressStat"].asString()));
	}
	return questdynamicdata;
}

FMerchantQuestSaveData parseMerchantQuestData(const Json::Value& node) {
	FMerchantQuestSaveData questdata;
	if (node.isMember("questState")) {
		questdata.mCountQuestState = parseMerchantCountQuestStateData(node["questState"]);
	}
	if (node.isMember("dynamicQuestState")) {
		questdata.mDynamicQuestState = parseMerchantDynamicQuestStateData(node["dynamicQuestState"]);
	}
	return questdata;
}

FMerchantPricingSaveData parseMerchantPricingData(const Json::Value& node) {
	FMerchantPricingSaveData pricingdata;
	pricingdata.mTimesRestocked = node["timesRestocked"].asInt(0);
	return pricingdata;
}

FMerchantSaveData parseMerchantData(const Json::Value& node) {
	FMerchantSaveData merchant;
	merchant.mEverInteracted = node["everInteracted"].asBool(false);
	merchant.mSlots = json::common::JsonMapToTMap<FName, FMerchantSlotSaveData>(node["slots"], RETLAMBDA(stringutil::toFName(it)), RETLAMBDA(parseMerchantSlotData(it)));
	merchant.mQuests = json::common::JsonMapToTMap<FName, FMerchantQuestSaveData>(node["quests"], RETLAMBDA(stringutil::toFName(it)), RETLAMBDA(parseMerchantQuestData(it)));
	merchant.mPricing = parseMerchantPricingData(node["pricing"]);
	return merchant;
}

TOptional<CompletedTrialData> parseCompletedTrial(const Json::Value& object) {
	if (!object.isMember("id")) {
		return {};
	}
	const auto id = stringutil::toFString(object["id"].asString("undefined"));
	const auto difficultyString = stringutil::toFString(object["difficulty"].asString());
	const auto difficulty = GetEnumValueFromStringT<EGameDifficulty>(difficultyString).Get(EGameDifficulty::Difficulty_1);
	return CompletedTrialData{ id, difficulty };
}


//missionState
FMissionDifficulty parseMissionDifficulty(const Json::Value& node) {
	FMissionDifficulty missionDifficulty;
	missionDifficulty.mission = GetEnumValueFromStringT<ELevelNames>(stringutil::toFString(node["mission"].asString())).Get(ELevelNames::Invalid);
	missionDifficulty.difficulty = GetEnumValueFromStringT<EGameDifficulty>(stringutil::toFString(node["difficulty"].asString())).Get(EGameDifficulty::Difficulty_1);
	missionDifficulty.threatLevel = GetEnumValueFromStringT<EThreatLevel>(stringutil::toFString(node["threatLevel"].asString())).Get(EThreatLevel::Threat_1);
	missionDifficulty.endlessStruggle = { node["endlessStruggle"].asInt(0) };
	return missionDifficulty;
}

FMissionState parseMissionState(const Json::Value& node) {
	FMissionState missionState;
	missionState.missionDifficulty = parseMissionDifficulty(node["missionDifficulty"]);
	for (const auto& itemNode : node["offeredItems"]) {
		if(auto maybeItem = parseInventoryItemData(itemNode)){
			missionState.offeredItems.Add(maybeItem.GetValue());
		}
	}
	for (const auto& itemNode : node["ownedDLCs"]) {
		if(auto dlc = parseDLC(itemNode)){
			missionState.ownedDLCs.Add(dlc.GetValue());
		}
	}
	missionState.offeredEnchantmentPoints = node["offeredEnchantmentPoints"].asInt(0);
	missionState.livesLost = node["livesLost"].asInt(0);
	missionState.partsDiscovered = node["partsDiscovered"].asInt(0);
	missionState.guid = stringutil::toFString(node["guid"].asString());
	missionState.seed = node["seed"].asInt(0);
	missionState.bCompletedOnce = node["completedOnce"].asBool(false);
	return missionState;
}


}}

//////////////////////////////////////////////////////////////////////////


#if CONSOLE_SAVE_SYSTEM



//D11.SC - Async Json Save Implementation
class FAsyncCharacterSaveDataJsonSave : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FAsyncCharacterSaveDataJsonSave>;

public:
	FAsyncCharacterSaveDataJsonSave(const UCharacterSaveData::RecordedData& SourceData, const FString& BaseFileName, int userIndex)
		:
		mRecordedData(SourceData),
		mBaseFilename(BaseFileName),
		mUserIndex(userIndex)
	{

	}

protected:
	UCharacterSaveData::RecordedData mRecordedData;
	FString mBaseFilename;
	int mUserIndex;

	void DoWork()
	{
		Json::Value node;

		mRecordedData.RecordSaveDataToJsonNode(node);

		//D11.PS - Console save
		Json::FastWriter writer;
		std::string json = writer.write(node);
		// D11.SSN
		SaveMetaData metaData;
		metaData.saveName = mBaseFilename;
		metaData.title = FText(LOCTEXT("Save_Character_Title", "Minecraft Dungeons - Character Save")).ToString();
		metaData.subTitle = FString::Printf(TEXT("%s: %s | %s: %d | %s: %d"), *FText(LOCTEXT("Save_Character_Date", "Creation Date")).ToString(), *mRecordedData.creationDate, *FText(LOCTEXT("Save_Character_Level", "Level")).ToString(), UDungeonsGameInstance::createCharacterLevel(mRecordedData.xp).level, *FText(LOCTEXT("Save_Character_Rating", "Gear Rating")).ToString(), mRecordedData.totalGearPower);
		metaData.details = FText(LOCTEXT("Save_Character_Detail", "The save file for one of your characters.")).ToString();
#if PLATFORM_PS4
		metaData.iconPath = TEXT("../../../dungeons/Content/SaveIcons/save_character.png");
#endif
		DungeonsConsoleSave::Instance()->SaveAsync(metaData, mUserIndex, json);

	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncGlobalStateDataJsonSave, STATGROUP_ThreadPoolAsyncTasks);
	}
};


#endif// CONSOLE_SAVE_SYSTEM


//////////////////////////////////////////////////////////////////////////

FName UCharacterSaveData::GenerateRandomDefaultSkin() const {
	const auto DefaultSkins = USkinsUtil::GetDefaultSkinIds(this);
	return DefaultSkins.Num() > 0 ? DefaultSkins[Util::sharedRandom().nextInt() % DefaultSkins.Num()] : NAME_None;	// random among the default skins
}

ItemSaveData::ItemSaveData(const FItemId& id, float itemPower, EItemRarity itemRarity, bool isUpgraded, bool isGifted, bool isModified, bool isEquipped, bool isMarkedNew,
	bool isCloned, EEquipmentSlot itemSlot, int32 index, std::vector<EnchantmentSaveData> itemEnchantments, std::vector<ArmorPropertySaveData> itemArmorProperties,
	TOptional<EnchantmentSaveData> netherite)
	: type(id)
	, power(itemPower)
	, rarity(itemRarity)
	, upgraded(isUpgraded)
	, gifted(isGifted)
	, modified(isModified)
	, equipped(isEquipped)
	, markedNew(isMarkedNew)
	, cloned(isCloned)
	, equipmentSlot(itemSlot)
	, inventoryIndex(index)
	, enchantments(std::move(itemEnchantments))
	, armorProperties(std::move(itemArmorProperties))
	, netheriteEnchant(std::move(netherite))
{
}

void  UCharacterSaveData::SetNewPlayerId()
{
	SetPlayerId(UKismetGuidLibrary::NewGuid());
}


void UCharacterSaveData::CreateNew(FString savePath) {
	mSavePathAndFilename = savePath;

	SetNewPlayerId();
	mRecordedData.skin = GenerateRandomDefaultSkin();
	mRecordedData.name = "";
	mRecordedData.customized = false;							// require character to be changed
	mRecordedData.creationDate = getCurrentDate(); // D11.SSN
}

void UCharacterSaveData::CreateNewShallowClone(const UCharacterSaveData& cloneSource, const FString& newSavePath) {
	mSavePathAndFilename = newSavePath;
	mRecordedData.isClone = true;
	SetNewPlayerId();

	mRecordedData.name = cloneSource.mRecordedData.name + " clone";
	mRecordedData.skin = cloneSource.mRecordedData.skin;
	mRecordedData.customized = cloneSource.mRecordedData.customized;
	mRecordedData.xp = cloneSource.mRecordedData.xp;
	mRecordedData.totalGearPower = cloneSource.mRecordedData.totalGearPower; // D11.SSN

	// only copy equipped, with value of zero.
	mRecordedData.items = algo::copy_if_map_vector(cloneSource.mRecordedData.items, RETLAMBDA(it.equipped), [](const ItemSaveData& it) {
		ItemSaveData dest = it;
		dest.cloned = true;
		return dest;
	});

	mRecordedData.cosmetics = cloneSource.mRecordedData.cosmetics;
	mRecordedData.cosmeticsEverEquipped = cloneSource.mRecordedData.cosmeticsEverEquipped;
	mRecordedData.trialsCompleted = cloneSource.mRecordedData.trialsCompleted;
	mRecordedData.progressionKeys = cloneSource.mRecordedData.progressionKeys;
	mRecordedData.difficulties = cloneSource.mRecordedData.difficulties;
	mRecordedData.threatLevels = cloneSource.mRecordedData.threatLevels;
	mRecordedData.mapUIState = cloneSource.mRecordedData.mapUIState;
	mRecordedData.unlockedSecretMissions = cloneSource.mRecordedData.unlockedSecretMissions;
	mRecordedData.progress = cloneSource.mRecordedData.progress;
	mRecordedData.unlockedLobbyChests = cloneSource.mRecordedData.unlockedLobbyChests;
	mRecordedData.creationDate = getCurrentDate(); // D11.SSN
	mRecordedData.strongholdProgress = cloneSource.mRecordedData.strongholdProgress;
}
void UCharacterSaveData::CreateNewDeepClone(const UCharacterSaveData& cloneSource, const FString& newSavePath ){
		mRecordedData = cloneSource.mRecordedData;
		mSavePathAndFilename = newSavePath;
}

void UCharacterSaveData::Load(FString savePath, int32 localUserNum) {
	mSavePathAndFilename = savePath;

#if CONSOLE_SAVE_SYSTEM
	//D11.PS - Console save system
	FString saveFilename = GetBaseFilename();

	ConsoleSaveFinishedLoadingDelegate = DungeonsConsoleSave::Instance()->OnAsyncLoadFinished.AddUObject(this, &UCharacterSaveData::OnConsoleSaveDataFinishedLoad);
	//D11.PS - This user index passed in will eventually have to be passed through
	TArray<uint8> saveData;
	DungeonsConsoleSave::Instance()->Load(saveFilename, localUserNum, saveData);
#else
	if (auto maybeJson = binarysavedata::safer::loadBinaryFileToJsonSafer(mSavePathAndFilename)) {
		AfterLoadSaveData(localUserNum, maybeJson.GetValue());
	} else {
		OnLoadFinished.Broadcast(false, localUserNum, this);
	}
#endif
}

void UCharacterSaveData::EnsurePathExists(FString inSavePathAndFilename) {
	// create PIE / Character folders if they dont exist
	FString path = FPaths::GetPath(inSavePathAndFilename);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*path)) {
		PlatformFile.CreateDirectoryTree(*path);
	}
}


void UCharacterSaveData::Save(int32 localUserNum) {
	using namespace json::character;
	using namespace json::common;
	if (markedForDelete)
	{
		return;
	}

	


#if CONSOLE_SAVE_SYSTEM

	//D11.SC Moved the Json processing off the game thread as it was costing a few MS
	(new FAutoDeleteAsyncTask<FAsyncCharacterSaveDataJsonSave>(mRecordedData, GetBaseFilename(), localUserNum))->StartBackgroundTask(DungeonsConsoleSave::Instance()->ConsoleSaveDataThreadPool.GetThreadPool());

#else

	Json::Value node;

	mRecordedData.RecordSaveDataToJsonNode(node);

	EnsurePathExists(mSavePathAndFilename);
	auto result = binarysavedata::safer::writeJsonToBinaryFileSafer(node, mSavePathAndFilename);

	if (result.regular != binarysavedata::safer::WriteResult::Saved) {
		if (mLastFailedFileWrite != mSavePathAndFilename && OnSaveFailed.IsBound())
		{
			OnSaveFailed.Broadcast(mSavePathAndFilename + ".tmp");
			mLastFailedFileWrite = mSavePathAndFilename;
		}
	}


#endif
}
#undef LOCTEXT_NAMESPACE

bool UCharacterSaveData::MoveFileTo(const FString& destinationPath) {
	EnsurePathExists(destinationPath);

	IFileManager& FileManager = IFileManager::Get();
	if (FileManager.Move(*destinationPath, *mSavePathAndFilename, true, true, true, true)) {
		mSavePathAndFilename = destinationPath;
		return true;
	}

	return false;
}

FString UCharacterSaveData::GetBaseFilename() {
	return FPaths::GetBaseFilename(mSavePathAndFilename);
}

bool UCharacterSaveData::HasCompletedAnyLevel() const {
	return mRecordedData.progress.size() > 0;
}

void UCharacterSaveData::addPendingRewardItem(const FRewardData& item) {
	mRecordedData.pendingRewardItems.Add(item);
}

TOptional<FRewardData> UCharacterSaveData::claimNextPendingRewardItem() {
	if (mRecordedData.pendingRewardItems.Num() > 0) {
		FRewardData reward = mRecordedData.pendingRewardItems[0];
		mRecordedData.pendingRewardItems.RemoveAt(0);
		return reward;
	}
	return {};
}

bool UCharacterSaveData::hasPendingRewardItem() const {
	return mRecordedData.pendingRewardItems.Num() > 0;
}

void UCharacterSaveData::addTrialCompleted(const FString& trialId, EGameDifficulty difficulty) {
	if (!hasCompletedTrial(trialId, difficulty)) {
		mRecordedData.trialsCompleted.push_back({ trialId, difficulty });
	}
	if (mRecordedData.trialsCompleted.size() > 100 * NumberOfDifficulties) {
		mRecordedData.trialsCompleted.erase(mRecordedData.trialsCompleted.begin());
	}
}

bool UCharacterSaveData::hasCompletedTrial(const FString& trialId, EGameDifficulty difficulty) const {
	return algo::contains(mRecordedData.trialsCompleted, CompletedTrialData{ trialId, difficulty });
}

bool UCharacterSaveData::clearTrial(const FString& trialId, EGameDifficulty difficulty) {
	if (const auto i = algo::index_of(mRecordedData.trialsCompleted, CompletedTrialData{ trialId, difficulty })) {
		mRecordedData.trialsCompleted.erase(mRecordedData.trialsCompleted.begin() + i.GetValue());
		return true;
	}
	return false;
}

void UCharacterSaveData::clearTrials() {
	mRecordedData.trialsCompleted.clear();
}

uint32 UCharacterSaveData::getCurrencyFor(const FItemId& inType) const {
	auto it = std::find_if(mRecordedData.currency.begin(), mRecordedData.currency.end(), [inType](const CurrencySaveData& v) { return v.type == inType; });
	if (it != mRecordedData.currency.end()) {
		return it->count;
	}
	return 0;
}

TArray<const FItemId*> UCharacterSaveData::getOwnedCurrencyTypes() const {
	return algo::map_tarray(mRecordedData.currency, RETLAMBDA(&it.type));
}

// D11.SSN
FString UCharacterSaveData::getCurrentDate() {
	// get current date
	FDateTime dateTime;
	dateTime = dateTime.Now();

	return FText::AsDate(dateTime.Now()).ToString();
}

void UCharacterSaveData::OnConsoleSaveDataFinishedLoad(bool bSuccessfull, FString filename, int32 userIndex, TArray<uint8>& saveBlob)
{
	if (!filename.Equals(GetBaseFilename()))
	{
		return;
	}

	if (!bSuccessfull)
	{
		OnLoadFinished.Broadcast(bSuccessfull, userIndex, this);
	}

	Json::Value node;
	Json::Reader reader;
#if CONSOLE_SAVE_SYSTEM
	const char *begining = (const char*)saveBlob.GetData();
	const char *end = begining + saveBlob.GetAllocatedSize();

	if (!reader.parse(begining, end, node))
	{
		OnLoadFinished.Broadcast(false, userIndex, this);
	}
#endif

	AfterLoadSaveData(userIndex, node);

}

void UCharacterSaveData::AfterLoadSaveData(int32 LocalUserNum, Json::Value &node)
{
	mRecordedData.ReadSaveDataFromJsonNode(node);
#if CONSOLE_SAVE_SYSTEM
	DungeonsConsoleSave::Instance()->OnAsyncLoadFinished.Remove(ConsoleSaveFinishedLoadingDelegate);
#endif
	//no skin, randomise
	if (mRecordedData.skin == NAME_None)
	{
		mRecordedData.skin = UCharacterSaveData::GenerateRandomDefaultSkin();
	}

	OnLoadFinished.Broadcast(true, LocalUserNum, this);
}

void UCharacterSaveData::RecordedData::RecordSaveDataToJsonNode(Json::Value& node) const
{
	using namespace json::common;
	using namespace json::character;

	node["timestamp"] = FDateTime::UtcNow().ToUnixTimestamp();
	node["version"] = SAVE_VERSION;
	node["name"] = stringutil::toStdString(name);
	node["customized"] = customized;
	node["clone"] = isClone;
	node["skin"] = stringutil::toStdString(skin.ToString());
	node["xp"] = xp;
	node["playerId"] = stringutil::toStdString(playerId.ToString(EGuidFormats::DigitsWithHyphens));
	node["pendingRewardItems"] = iterableToJsonArray(pendingRewardItems, RETLAMBDA(toJson(it)));
	node["items"] = iterableToJsonArray(items, RETLAMBDA(toJson(it)));
	node["cosmetics"] = iterableToJsonArray(cosmetics, RETLAMBDA(toJson(it)));
	node["cosmeticsEverEquipped"] = iterableToJsonArray(cosmeticsEverEquipped);
	node["itemsFound"] = iterableToJsonArray(itemsFound, RETLAMBDA(toJson(it)));
	node["currenciesFound"] = iterableToJsonArray(currenciesFound, RETLAMBDA(toJson(it)));
	node["uiHintsExpired"] = iterableToJsonArray(uiHintsExpired, RETLAMBDA(toJson(it)));
	node["currency"] = iterableToJsonArray(currency, RETLAMBDA(toJson(it)));
	node["trialsCompleted"] = iterableToJsonArray(trialsCompleted, RETLAMBDA(toJson(it)));
	node["progressionKeys"] = iterableToJsonArray(progressionKeys);
	node["progress"] = toJson(progress);
	node["difficulties"] = toJson(difficulties);
	node["threatLevels"] = toJson(threatLevels);
	node["endGameContentProgress"] = toJson(endGameContentProgress);
	node["mapUIState"] = toJson(mapUIState);
	node["lobbychest_progress"] = toJson(unlockedLobbyChests);
	node["mob_kills"] = toJson(mobKills);
	node["bonus_prerequisites"] = iterableToJsonArray(unlockedSecretMissions, RETLAMBDA(enumString(it)));
	node["finishedObjectiveTags"] = toJson(finishedObjectiveTags);
	node["progressStatCounters"] = TMapToJsonMap(progressStatCounters, RETLAMBDA(enumString(it)), RETLAMBDA(it));
	node["videosPlayed"] = iterableToJsonArray(videosPlayed);
	node["creationDate"] = stringutil::toStdString(creationDate); // D11.SSN
	node["totalGearPower"] = totalGearPower; // D11.SSN
	node["merchantData"] = TMapToJsonMap(merchantData, RETLAMBDA(stringutil::toStdString(it)), RETLAMBDA(toJson(it)));
	node["legendaryStatus"] = Json::Value(static_cast<uint8>(legendaryStatus)); // D11.SSN
	node["missionStatesMap"] = TMapToJsonMap(missionStates, RETLAMBDA(enumString(it)), RETLAMBDA(toJson(it)));
	node["strongholdProgress"] = toJson(strongholdProgress);

}

void UCharacterSaveData::RecordedData::ReadSaveDataFromJsonNode(Json::Value& node)
{
	using namespace json::character;

	version = node["version"].asInt(0);

	name = stringutil::toFString(node["name"].asString());
	isClone = node["clone"].asBool(false);
	customized = node["customized"].asBool(false);
	skin = node["skin"].isString() ? FName(*stringutil::toFString(node["skin"].asString())) : NAME_None;
	xp = node["xp"].asInt();
	bool success = false;
	UKismetGuidLibrary::Parse_StringToGuid(stringutil::toFString(node["playerId"].asString()), playerId, success);

	if (node.isMember("creationDate")) {
		creationDate = stringutil::toFString(node["creationDate"].asString());
	}

	if (node.isMember("totalGearPower")) {
		totalGearPower = node["totalGearPower"].asUInt();
	}

	if (node.isMember("legendaryStatus") && node["legendaryStatus"].asUInt() < 3) {
		legendaryStatus = static_cast<ELegendaryStatus>(node["legendaryStatus"].asUInt());
	}

	if (node.isMember("strongholdProgress")) {
		strongholdProgress = parseStrongholdProgress(node["strongholdProgress"]);
	}

	for (const auto& item : node["items"]) {
		if(const auto saveItem = parseItem(item)) {
			items.push_back(saveItem.GetValue());
		}
	}

	for (const auto& cosmetic : node["cosmetics"]) {
		cosmetics.Add(parseCosmetic(cosmetic));
	}

	for (const auto& cosmeticId : node["cosmeticsEverEquipped"]) {
		cosmeticsEverEquipped.Add(FName(*stringutil::toFString(cosmeticId.asString())));
	}

	for (const auto& uiHint : node["uiHintsExpired"]) {
		if (auto hintProgress = parseUIHintProgress(uiHint)) {
			uiHintsExpired.push_back(hintProgress.GetValue());
		}
	}

	for (const auto& foundItem : node["itemsFound"]) {
		if (auto enumType = parseItemId(foundItem)) {
			itemsFound.Add(enumType.GetValue());
		}
	}

	for (const auto& foundCurrency : node["currenciesFound"]) {
		if (auto enumType = parseItemId(foundCurrency)) {
			currenciesFound.Add(enumType.GetValue());
		}
	}

	for (const auto& currencyItem : node["currency"]) {
		if(auto c = parseCurrencyItem(currencyItem)) {
			currency.push_back(c.GetValue());
			const auto& type = GetItemRegistry().Get(c->type);
			if (type.hasTag(ItemTag::Currency)) {
				currenciesFound.Add(c->type);
			}
		}
	}

	auto&& jsonProgress = node["progress"];
	for (const auto& missionId : jsonProgress.getMemberNames()) {
		progress[level::fromString(stringutil::toFString(missionId))] = parseMissionProgress(jsonProgress[missionId]);
	}

	if (node.isMember("difficulties")) {
		difficulties = parseDifficulties(node["difficulties"]);
	}
	else {
		difficulties = {};
	}

	if (node.isMember("threatLevels")) {
		threatLevels = parseThreatLevels(node["threatLevels"]);
	}
	else {
		threatLevels = {};
	}

	if (node.isMember("endGameContentProgress")) {
		endGameContentProgress = parseEndGameContentProgress(node["endGameContentProgress"]);
	}
	else {
		endGameContentProgress = {};
	}

	if (node.isMember("mapUIState")) {
		mapUIState = parseMapUIState(node["mapUIState"]);
	}
	else {
		mapUIState = {};
	}

	auto&& lobbychestProgress = node["lobbychest_progress"];
	for (const auto& chestIdString : lobbychestProgress.getMemberNames()) {
		const auto chestId = std::atoi(chestIdString.c_str());
		unlockedLobbyChests[chestId] = parseLobbyChestProgress(lobbychestProgress[chestIdString]);
	}

	for (const auto& mobEntityType : node["mob_kills"].getMemberNames()) {
		mobKills[EntityTypeFromString(mobEntityType)] = node["mob_kills"][mobEntityType].asInt();
	}

	if (node.isMember("pendingRewardItem")) {
		if (const auto pendingRewardItem = parsePendingRewardItem(node["pendingRewardItem"])) {
			pendingRewardItems.Add(FRewardData(ERewardType::LevelCompletion, pendingRewardItem.GetValue()));
		}
	}
		

	if (node.isMember("pendingRewardItems")) {
		for (auto&& reward : node["pendingRewardItems"]) {
			if (const auto pendingRewardItem = parsePendingRewardData(reward)) {
				pendingRewardItems.Add(pendingRewardItem.GetValue());
			}
		}
	}

	for (const auto& completedTrialNode : node["trialsCompleted"]) {
		if (const auto completedTrial = parseCompletedTrial(completedTrialNode)) {
			trialsCompleted.push_back(completedTrial.GetValue());
		}
	}

	unlockedSecretMissions = {};
	for (auto&& levelName : node["bonus_prerequisites"]) {
		unlockedSecretMissions.Add(level::fromString(stringutil::toFString(levelName.asString())));
	}

	for (const auto& progressionKey : node["progressionKeys"]) {
		progressionKeys.Add(stringutil::toFString(progressionKey.asString()));
	}

	auto&& tags = node["finishedObjectiveTags"];
	for (const auto& tag : tags.getMemberNames()) {
		finishedObjectiveTags[tag] = tags[tag].asInt();
	}

	if(node.isMember("progressStatCounters")){
		progressStatCounters = parseProgressStatCounters(node["progressStatCounters"]);
	}

	for (const auto& videoId : node["videosPlayed"]) {
		videosPlayed.Add(stringutil::toFString(videoId.asString()));
	}

	if (node.isMember("merchantData")) {
		merchantData = json::common::JsonMapToTMap<FName, FMerchantSaveData>(node["merchantData"], RETLAMBDA(stringutil::toFName(it)), RETLAMBDA(parseMerchantData(it)));
	}

	if (node.isMember("missionStatesMap")) {
		missionStates = json::common::JsonMapToTMap<ELevelNames, FMissionState>(node["missionStatesMap"], RETLAMBDA(level::fromString(stringutil::toFString(it))), RETLAMBDA(parseMissionState(it)));
	}

	if (version != SAVE_VERSION) {

		//Merchant have been updated
		//Convert old completed missions to unlocked merchants
		if (version <= 1) {
			for (auto& merchantdefclass : merchantdefs::getAllEnabledClasses()) {
				auto def = merchantdefclass->GetDefaultObject<UMerchantDef>();
				check(def && "failed to get default object");
				if (auto legacyUnlockMission = def->GetLegacyUnlockMission()) {
					if (progress.find(legacyUnlockMission.GetValue()) != progress.end()) {
						if (progress[legacyUnlockMission.GetValue()].completedDifficulty.Get(EGameDifficulty::Invalid) >= difficultyquery::First) {
							progressionKeys.Add(def->GetUnlockProgressKey());
						}
					}
				}
			}
		}

		//Endless struggle has been updated
		if(version <= 2) {
			FEndlessStruggle max;

			//New progress is old progress /2...
			for(auto& mission : progress) {
				mission.second.completedEndlessStruggle.Value /= 2;
				max = mission.second.completedEndlessStruggle > max ? mission.second.completedEndlessStruggle : max;
			}

			const auto tiers = endlesstruggle::tier::getAllLowestToHighest();
			const auto unlockedTier =  endlesstruggle::tier::getUnlockFor(max);
			int requiredNumBosses = -1;
			FEndlessStruggle requiredBossCompletionLevel;

			//Add Progress keys, find required number of bosses and endless struggle required
			for(const auto* tier : tiers) {
				requiredNumBosses++;

				if(tier == unlockedTier) {
					break;
				}
				requiredBossCompletionLevel = tier->unlockedEndlessStruggle();
			}

			//Make a list of all boss missions, with current progress set
			TArray<FMissionProgress> data = algo::map_tarray(missions::getAllCurrentlyEnabledMatching(RETLAMBDA(it.hasBoss())), 
				[&](const MissionDef* it) -> FMissionProgress {
					FMissionProgress data;
					data.levelName = it->level();
					auto existing = progress.find(data.levelName);
					if(existing != progress.end()) {
						data.completedDifficulty = existing->second.completedDifficulty.Get(EGameDifficulty::Invalid);
						data.completedThreatLevel = existing->second.completedThreatLevel.Get(EThreatLevel::Invalid);
						data.completedEndlessStruggle = existing->second.completedEndlessStruggle;
					}

					return data;
				}
			);

			//Sort with highest completed on top.
			data.Sort([](const FMissionProgress& a, const FMissionProgress& b) {
				if(a.completedDifficulty != b.completedDifficulty) {
					return a.completedDifficulty > b.completedDifficulty;
				} else if(a.completedThreatLevel != b.completedThreatLevel) {
					return a.completedThreatLevel > b.completedThreatLevel;
				} else {
					return a.completedEndlessStruggle > b.completedEndlessStruggle;
				}
			});

			//Fudge progress to match requirements
			for(int i = 0; requiredNumBosses > 0 && i < data.Num(); ++i) {
				auto& mission = data[i];
				MissionProgressSaveData missionProgress;
				missionProgress.completedEndlessStruggle.Value = FMath::Max(mission.completedEndlessStruggle.Value, requiredBossCompletionLevel.Value);
				missionProgress.completedDifficulty = difficultyquery::RequiredForEndlessStuggle;
				missionProgress.completedThreatLevel = threatquery::RequiredForEndlessStuggle;

				auto existing = progress.find(mission.levelName);
				if (existing != progress.end()) {
					existing->second = missionProgress;
				} else {
					progress.emplace(std::piecewise_construct,
						std::forward_as_tuple(mission.levelName),
						std::forward_as_tuple(missionProgress)
					);
				}

				--requiredNumBosses;
			}
		}

		version = SAVE_VERSION;
	}
}

void UCharacterSaveData::SetLegendaryStatus(bool legendary) {
	if (legendary) {
		mRecordedData.legendaryStatus = ELegendaryStatus::LEGENDARY;
	}
	else {
		if (mRecordedData.legendaryStatus == ELegendaryStatus::LEGENDARY) {
			mRecordedData.legendaryStatus = ELegendaryStatus::MYTHICAL;
		}
	}
}

ELegendaryStatus UCharacterSaveData::GetLegendaryStatus() {
	return mRecordedData.legendaryStatus;
}

const FRewardData& UCharacterSaveData::getNextPendingRewardItem() {
	checkf(hasPendingRewardItem(), TEXT("Trying to get a non existing reward item"));
	return mRecordedData.pendingRewardItems[0];
}

int UCharacterSaveData::numPendingRewards() const {
	return mRecordedData.pendingRewardItems.Num();
}

ERewardType UCharacterSaveData::getNextPendingRewardItemRewardType() {
	return getNextPendingRewardItem().RewardType;
}

bool StrongholdProgressSaveData::IsUnlocked(EEyeOfEnderType type) const {
	const auto* progress = progresses.Find(type);
	return eyeofenderquery::isValid(type) && progress && progress->unlocked;
}

bool StrongholdProgressSaveData::HasEye(EEyeOfEnderType type) const {
	const auto* progress = progresses.Find(type);
	return eyeofenderquery::isValid(type) && progress && progress->hasEye;
}

int StrongholdProgressSaveData::EyesHeldCount() const {
	return algo::count_if(progresses, RETLAMBDA(it.Value.hasEye));
}

int StrongholdProgressSaveData::EyesPlacedInPortalCount() const {
	return algo::count_if(progresses, RETLAMBDA(it.Value.unlocked));
}

bool StrongholdProgressSaveData::AreAllEyesPlacedInPortal() const {
	return EyesPlacedInPortalCount() == TOTAL_EYES_OF_ENDER;
}

bool StrongholdProgressSaveData::AllowsPickup(EEyeOfEnderType type) const {
	const auto* progress = progresses.Find(type);
	return eyeofenderquery::isValid(type) && (!progress || (!progress->hasEye && !progress->unlocked));
}

void StrongholdProgressSaveData::GiveEye(EEyeOfEnderType type) {
	check(eyeofenderquery::isValid(type));
	progresses.FindOrAdd(type).hasEye = true;
}

void StrongholdProgressSaveData::UnlockAndRemoveEye(EEyeOfEnderType type) {
	check(eyeofenderquery::isValid(type));
	auto& progress = progresses.FindOrAdd(type);
	progress.unlocked = true;
	progress.hasEye = false;
}
