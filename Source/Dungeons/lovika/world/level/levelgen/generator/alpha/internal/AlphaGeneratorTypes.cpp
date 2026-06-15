#include "Dungeons.h"
#include "AlphaGeneratorTypes.h"
#include "AlphaGeneratorDeadEnds.h"
#include "lovika/BlockPosTransform.h"
#include "lovika/tile/TilePlacement.h"
#include "lovika/tile/TilePredicates.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "lovika/world/level/levelgen/generator/GeneratorGraphUtil.h"
#include "util/CollectionUtils.h"
#include "util/FloatWeighedRandom.h"
#include "util/SharedFunc.h"

namespace generator { namespace alpha {

//
// TileGetters
//
namespace tilegetters {

const StretchTileProvider& None() {
	static const StretchTileProvider provider = [](const GenState&) { return TOptional<MetaTile>{}; };
	return provider;
}

StretchTileProvider Default(MetaTileVector metaTiles) {
	if (metaTiles.empty()) {
		return None();
	}
	const auto totalWeight = FloatWeighedRandom::getTotalWeight(metaTiles);
	return [tiles = std::move(metaTiles), totalWeight](const GenState& state) {
		return *FloatWeighedRandom::getRandomItem(&state.rnd, tiles, totalWeight);
	};
}

}

//
// StretchCompletion queries
//
namespace completionqueries {
StretchCompletion LengthInTiles(int length) {
	return [length](const GenState& state) { return state.currentStretchIndex >= length; };
}
}

std::vector<StretchData> createDefaultStretchData(const Env& env, const std::vector<io::Stretch>& ioStretches) {
	const auto sharedDungeonSelector = makeSharedFunc1(env.dungeonSelector);
	const auto sharedDungeonBatchSelector = env.dungeonBatchSelector ? makeSharedFunc1(env.dungeonBatchSelector) : DungeonBatchSelector{};
	DeadEndsPerLevel deadEndsPerLevel(env);

	std::vector<StretchData> stretches;
	for (auto& ioStretch : ioStretches) {
		stretches.push_back({
			ioStretch,
			completionqueries::LengthInTiles(ioStretch.length),
			tilegetters::Default(createMetaTiles(env.tileGroup, ioStretch.tiles)),
			ioStretch.deadEnds ?
				createMetaTiles(env.tileGroup, ioStretch.deadEnds.GetValue()) :
				deadEndsPerLevel.getOrCreate(env.dungeons[ioStretch.dungeonIndex].level),
			sharedDungeonSelector,
			sharedDungeonBatchSelector
		});
	}
	return stretches;
}

//
// GenState
//
GenState::GenState(const Env& env, const std::vector<io::Stretch>& ioStretches, RandomSeed seed)
	: env(env)
	, stretches(createDefaultStretchData(env, ioStretches))
	, rnd(seed)
	, stretchCount(stretches.size())
	, intersector(true)
	, stretchLengths(stretches.size()) {
}

const StretchData& GenState::stretchDef() const {
	return stretches[stretchIndex];
}

const StretchData* GenState::stretchDefForTileIndex(int tileIndex) const {
	return Util::isValidIndex(stretchIds, tileIndex) ? &stretches[stretchIds[tileIndex].index] : nullptr;
}

const io::Dungeon* GenState::dungeonForTileIndex(int tileIndex) const {
	if (const auto* stretchData = stretchDefForTileIndex(tileIndex)) {
		return &env.dungeons[stretchData->stretch.dungeonIndex];
	}
	return nullptr;
}

void GenState::place(PlaceResult p, StretchId stretchId) {
	intersector.add(p.tilePlacement.bounds());
	placed.add(std::move(p));
	stretchIds.push_back(std::move(stretchId));
}

BlockPos calculateOffsiteTranslation(const PlacedTiles& base, const PlacedTiles& offsite) {
	const auto previousBounds = base.bounds(); // e.g. (10,20,30) - (100, 200, 300)
	if (previousBounds.isEmpty()) {
		return {};
	}
	const auto bounds = offsite.bounds(); // e.g. (4, 6, 8) - (200, 204, 208)
	const int marginX = 100;

	return { // Translate away in X only, align (Y,Z) to min values
		previousBounds.maxExclusive.x - bounds.minInclusive.x + marginX,
		previousBounds.minInclusive.y - bounds.minInclusive.y,
		previousBounds.minInclusive.z - bounds.minInclusive.z
	};
}

void GenState::placeOffsite(TOptional<DoorPair> from, const GenState& offsite) {
	//check(!result.tileInfos[0].placeResult.doorPairs.empty() && "The offsite group needs to have an entry door");
	const int baseStretchOffset = static_cast<int>(stretches.size());
	for (size_t i = 0; i < offsite.stretches.size(); ++i) {
		stretches.push_back(offsite.stretches[i]);
		stretchLengths.push_back(offsite.stretchLengths[i]);
	}
	const BlockPos translation = calculateOffsiteTranslation(placed, offsite.placed);
	std::vector<PlaceResult> placeResults = offsite.placed.results();
	for (auto& placeResult : placeResults) {
		translate(placeResult, translation);
	}
	// Connect an edge between the given door and our entry door.
	if (from) {
		from->to = transformed(from->to, blockpostransform::offset(translation));

		PlaceResult& entryResult = placeResults[0];
		if (entryResult.doorPairs.empty()) { // @attn @note we used to make sure list isn't empty before
			entryResult.doorPairs.push_back(std::move(from.GetValue()));
		} else {
			entryResult.doorPairs[0] = std::move(from.GetValue());
		}
	}

	for (size_t i = 0; i < offsite.placed.size(); ++i) {
		const auto& stretch = offsite.stretchIds[i];
		place(placeResults[i], {
			stretch.index + baseStretchOffset,
			stretch.mainPathIndex,
			stretch.isDefinitelyOffMainPath,
			1 + currentDungeonInstanceIndex + stretch.dungeonInstance
		}); // @stretchid
	}
	if (!offsite.placed.isEmpty()) {
		currentDungeonInstanceIndex += algo::max_element_by(offsite.stretchIds, RETLAMBDA(it.dungeonInstance))->dungeonInstance + 1;
	}
}

Result GenState::makeResult() const {
	checkf(placed.size() == stretchIds.size(), TEXT("PlacedTiles and tileinfos are different length!"));

	Result result;
	result.seed = rnd.getSeed();
	result.issues = issues;

	std::unordered_map<int, int> dungeonInstanceByStretchIndex;
	for (auto& stretchId : stretchIds) {
		dungeonInstanceByStretchIndex[stretchId.index] = stretchId.dungeonInstance;
	}
	for (size_t i = 0; i < stretchLengths.size(); ++i) {
		result.stretches.push_back({
			stretches[i].stretch,
			static_cast<int>(i),
			stretchLengths[i],
			dungeonInstanceByStretchIndex[i]
		});
	}
	for (size_t i = 0; i < placed.size(); ++i) {
		const auto& placeResult = placed.result(i);
		const auto& metaTile = *env.tileGroup.findById(placeResult.tilePlacement.metaIdHACK());
		result.tileInfos.push_back({ metaTile, placeResult, stretchIds[i] });
	}
	result.graph = graph::util::createGraph(result.tileInfos);
	return result;
}


//
// GenStateRunner
//
GenStateRunner::GenStateRunner(GenState& state)
	: s(state) {
}

void GenStateRunner::goToNextStretch() {
	s.stretchLengths[s.stretchIndex] = s.currentStretchIndex;
	++s.stretchIndex;
	s.currentStretchIndex = 0;

	nexts++;
}

void GenStateRunner::goToPreviousStretch() {
	--s.stretchIndex;
	s.currentStretchIndex = s.stretchLengths[s.stretchIndex] - 1;

	prevs++;
}

bool GenStateRunner::isDone() const {
	return s.stretchIndex >= (int)s.stretches.size();
}

void GenStateRunner::push(PlaceResult p) {
	pushs++;

	s.place(p, { s.stretchIndex });

	s.currentStretchIndex++;
	s.globalTileIndex++;
}

void GenStateRunner::pop(int count/*= 1*/) {
	count = s.placed.pop_back_safe(count);
	/*count =*/ s.intersector.pop_back_safe(count);
	s.stretchIds.erase(s.stretchIds.end() - count, s.stretchIds.end());
	//s.placed.erase(s.placed.end() - count, s.placed.end()); //@placed

	pops += count;

	for (int i = 0; i < count; ++i) {
		s.currentStretchIndex--;
		s.globalTileIndex--;

		if (s.currentStretchIndex < 0) {
			goToPreviousStretch();
		}
	}
}

void GenStateRunner::setMainPathDone() const {
	s.mainPathLength = s.placed.size();
}

void GenStateRunner::printStats() const {
	UE_LOG(LogDungeons, Log, TEXT("Push/pop/next/prev: %d/%d/%d/%d (tileDt: %d, stretchDt: %d) -- %d / %d\nLengths:\n"), pushs, pops, nexts, prevs, (pushs - pops), (nexts - prevs), s.placed.size(), s.globalTileIndex);
	for (auto i : range(4)) {
		UE_LOG(LogDungeons, Log, TEXT("\t%d: %d\n"), i, s.stretchLengths[i]);
	}
}

//
// Errors -- Generator Stuck problem types
//
GsStretch::GsStretch(const io::Stretch& stretch)
	: index(stretch.indexInFile)
	, dungeonIndex(stretch.dungeonIndex)
	, id(stretch.id) {
}

GsNoTile::GsNoTile(const io::Stretch& stretch)
	: stretch(stretch) {
}

std::size_t GsNoTile::hashCode() const {
	std::size_t hash = 13;
	hash_combine(hash, stretch.index);
	hash_combine(hash, stretch.id);
	return hash;
}

bool GsNoTile::operator==(const GsNoTile& rhs) const {
	return stretch.index == rhs.stretch.index
		&& stretch.id == rhs.stretch.id;
}

GsNoPlacement::GsNoPlacement(Type type, const io::Stretch& stretch, const TOptional<TilePlacement>& previous, MetaTile tile)
	: type(type)
	, stretch(stretch)
	, previousTileName(previous ? previous->tileIdHACK() : TOptional<std::string>{})
	, tile(std::move(tile)) {
}

std::size_t GsNoPlacement::hashCode() const {
	std::size_t hash = 13; // @todo: return make_hash(a, b, c, ...);
	hash_combine(hash, type);
	hash_combine(hash, stretch.index);
	hash_combine(hash, stretch.id);
	hash_combine(hash, tile.lowerId);
	hash_combine(hash, previousTileName);
	return hash;
}

std::string createReasonString(const GsNoPlacement& it) {
	switch (it.type) {
	case GsNoPlacement::Type::IntersectsGeometry: return "intersects other tiles";
	case GsNoPlacement::Type::NoCandidates: return "no free and matching doors";
	default: checkNoEntry(); return "";
	}
}

std::string GsNoPlacement::toString() const {
	return "Couldn't place '" + tile.id + "' after '" + previousTileName.Get("<None: First tile>") + "'. Reason: " + createReasonString(*this);
}

bool GsNoPlacement::operator==(const GsNoPlacement& rhs) const {
	return type == rhs.type
		&& stretch.index == rhs.stretch.index
		&& stretch.id == rhs.stretch.id
		&& previousTileName == rhs.previousTileName
		&& tile.lowerId == rhs.tile.lowerId;
}

//
// Errors - List of failed placements (when generator gets stuck multiple times)
//
void GeneratorStuck::add(const GsNoTile& noTile) {
	noTiles[noTile]++;
}

void GeneratorStuck::add(const GsNoPlacement& noPlacement) {
	noPlacements[noPlacement]++;
}

template <typename GsType>
void addIfValue(GeneratorStuck& gs, const TOptional<GsType>& maybeValue) {
	if (maybeValue) {
		gs.add(maybeValue.GetValue());
	}
}

void GeneratorStuck::add(const GsFailedPlacement& failed) {
	addIfValue(*this, failed.noTile);
	addIfValue(*this, failed.noPlacement);
}

size_t GeneratorStuck::errorCount() const {
	return algo::sum(noTiles, RETLAMBDA(it.second))
	     + algo::sum(noPlacements, RETLAMBDA(it.second));
}

void GeneratorStuck::append(const GeneratorStuck& stuck) {
	for (auto& nt : stuck.noTiles) {
		noTiles[nt.first] += nt.second;
	}
	for (auto& np : stuck.noPlacements) {
		noPlacements[np.first] += np.second;
	}
}

bool GeneratorStuck::isEmpty() const {
	return noTiles.empty() && noPlacements.empty();
}

//
// Errors - Holder of all found generation errors
//
void GenerationError::append(const GenerationError& errors) {
	stuck.append(errors.stuck);
	::append(issues, errors.issues);
}

}}
