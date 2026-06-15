#include "Dungeons.h"
#include "AlphaGeneratorDeadEnds.h"
#include "lovika/tile/TilePredicates.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "util/RandomUtil.h"

namespace generator { namespace alpha {

bool placeDeadEndOnDoor(GenState& state, int tileIndex, const DoorDef& door, const MetaTile& stub) {
	const auto placement = util::canBePlaced(door, stub.tile().doors()[0], stub, state.env.config.maxDoorWidthDifference());

	if (placement && !state.intersector(placement->tilePlacement.bounds())) {
		state.place(placement.GetValue(), copyAsOffMainPathStretchId(state.stretchIds[tileIndex]));
		return true;
	}
	return false;
}

bool placeAnyDeadEndOnDoor(GenState& state, int tileIndex, const DoorDef& door, const std::vector<MetaTile>& stubs) {
	TArray<float> weights;
	for (auto& metaTile : stubs) {
		weights.Add(metaTile.weight);
	}
	for (const auto i : Util::weightedIndexShuffle(weights, &state.rnd)) {
		if (placeDeadEndOnDoor(state, tileIndex, door, stubs[i])) {
			return true;
		}
	}
	return false;
}

void generateDeadEnds(GenState& state, int tileIndex, const std::vector<MetaTile>& stubs) {
	if (stubs.empty()) {
		return;
	}
	for (auto&& door : state.placed[tileIndex].doors()) {
		if (state.placed.isConnected(door)) {
			continue;
		}
		auto doorTags = fromCommaSeparatedString(door.lowerTagString());
		if (!doorTags.has_alreadyLowerCase(DeadEndTag)) {
			continue;
		}
		placeAnyDeadEndOnDoor(state, tileIndex, door, stubs);
	}
}

void generateDeadEnds(GenState& state) {
	for (size_t i = 0; i < state.placed.size(); ++i) {
		const int stretchIndex = state.stretchIds[i].mainPathIndex;
		generateDeadEnds(state, i, state.stretches[stretchIndex].deadEnds);
	}
}

DeadEndsPerLevel::DeadEndsPerLevel(const Env& env)
	: mEnv(env) {
}

const MetaTileVector& DeadEndsPerLevel::getOrCreate(const io::DungeonLevelInfo& level) {
	const auto* existing = mLevelDeadEnds.Find(level.id);
	return existing ? *existing : mLevelDeadEnds.Add(level.id, mEnv.tileGroup.filter(tilepredicates::isDeadEndFromObjectGroups(level.lowerCaseObjectGroups)).tiles());
}

}}
