#include "Dungeons.h"
#include "HyperSpawner.h"
#include "game/Game.h"
#include "game/GameSettings.h"
#include "game/item/ItemType.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionDLCUtil.h"
#include "game/mission/hyper/HyperMissionUtil.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "lovika/io/IoHyperTypes.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"

namespace game { namespace mobspawn {

namespace internal {

std::set<ELevelNames> getExcludedMissions(const FMissionState& missionState) {
	std::set<ELevelNames> excluded;
	if (algo::none_of(missionState.offeredItems, RETLAMBDA(it.GetItemId() == item::type::RainbowGrass.getId()))) {
		excluded.insert(ELevelNames::mooshroomisland);
	}
	for (const auto excludedMission : missions::getAllCurrentlyEnabledMatching(game::mission::dlc::isExcludedMissionPredicate(missionState.getEligibleDLCs()))) {
		excluded.insert(excludedMission->level());
	}
	return excluded;
}

TArray<ELevelNames> getMissionsForItem(const ItemType& itemType, const std::set<ELevelNames>& excludedLevels) {
	const auto restrictedToLevels = [&] {
		const auto isNoVariation = [](ELevelNames it) { return !UMissionDefs::IsVariation(it) && !missions::get(it).isHyperMission(); };
		static const auto defaultMissions = algo::map_if_tarray(missions::getAllCurrentlyEnabled(), RETLAMBDA(it->level()), isNoVariation);
		const auto itemRestrictedMissions = algo::map_if_tarray(itemType.getRestrictedMissionDifficulties(), RETLAMBDA(it.Key), isNoVariation);
		return itemRestrictedMissions.Num() ? itemRestrictedMissions : defaultMissions;
	}();
	return algo::copy_if(restrictedToLevels, RETLAMBDA(excludedLevels.count(it) == 0));
}

std::vector<io::MobType> randomlyPruneLargeMobGroups(const std::vector<io::MobType>& types, Random& rnd) {
	const int maxTypeCountWithoutPruning = 6;
	const int prunedCount = FMath::FloorToInt(FMath::Pow(types.size(), 0.75f));
	return util::randomSample(types, FMath::Max(maxTypeCountWithoutPruning, prunedCount), rnd);
}

}

HyperSpawner::HyperSpawner(
	const game::Game& game,
	const game::DifficultyStats& difficultyStats,
	const FMissionState& missionState,
	Config spawnConfig
)
	: AlphaSpawner(game.world(), difficultyStats, spawnConfig)
{
	Random rnd(game.settings().randomSeed);

	const auto getRandomMissionForItem = [&, excluded = internal::getExcludedMissions(missionState)](const FInventoryItemData& itemData) {
		return Util::randomChoiceOrEmpty(internal::getMissionsForItem(itemData.GetItemType(), excluded), &rnd);
	};
	auto levelMobs = [&]() -> std::unordered_map<ELevelNames, io::HyperMobGroup> { // non-const due to levelMobs[] below for readability
		if (auto hyperLevel = levelgen::sourcedata::hyperLevelFromFileSystem(game.missionDef().levelFilename())) {
			return hyperLevel->hyperLevelMobs;
		}
		return {};
	}();

	for (const auto& subMissionId : util::getUniqueSubMissionIds(game.tiles())) {
		struct MobTypeExprCmp { bool operator() (const io::MobType& a, const io::MobType& b) const { return a.expr < b.expr; } };
		std::set<io::MobType, MobTypeExprCmp> mobTypes;

		for (auto& itemMission : algo::opt::unpack_tarray(algo::map_tarray(missionState.offeredItems, getRandomMissionForItem))) {
			algo::map_cast_to(levelMobs[itemMission].types, mobTypes);
		}

		io::MobGroup mobGroup;
		mobGroup.types = internal::randomlyPruneLargeMobGroups(algo::map_vector(mobTypes, RETLAMBDA(it)), rnd);
		mobGroup.typesCountInterval = { 2, 4 };
		prepareMobGroup(mobGroup);
		mMobGroups.push_back(mobGroup);
	}
}

CalculatedMobs HyperSpawner::calculateMobs(tile::TilePreparationState state) const {
	auto& maybeSubMissionMobGroup = mSubMissionMobGroups.FindOrAdd(state.tile.dungeon().def().level.id);
	return {
		algo::opt::get_or_set_compute(maybeSubMissionMobGroup, RETLAMBDA0(calculateSubMissionMobGroups(state))),
		calculateDefaultMobCountForTile(state, state.tile.stretch().def.mobs.density)
	};
}

std::vector<io::MobGroup> HyperSpawner::calculateSubMissionMobGroups(tile::TilePreparationState state) const {
	const auto contains = [](const std::string& haystack, const std::string& needle) {
		return haystack.find(needle) != std::string::npos;
	};
	const auto shouldKeepMobType = [&contains, isOverworld = state.tile.dungeon().def().dimension == std::string("overworld")](const std::string& expr) {
		if (isOverworld && contains(expr, "piglin") && !contains(expr, "zombified")) {
			return false;
		}
		return true;
	};
	auto mobGroup = mMobGroups[mMobGroupIndex++];
	mobGroup.types = algo::copy_if(mobGroup.types, RETLAMBDA(shouldKeepMobType(Util::toLower(it.expr))));
	return { mobGroup };
}

}}
