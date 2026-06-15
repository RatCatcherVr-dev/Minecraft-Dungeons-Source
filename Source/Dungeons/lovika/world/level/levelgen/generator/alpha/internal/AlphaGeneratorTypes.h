#pragma once

#include "CommonTypes.h"
#include "AlphaGeneratorTeleports.h"
#include "lovika/world/level/levelgen/generator/Generator.h"
#include "lovika/world/level/levelgen/generator/VolumesIntersector.h"
#include "lovika/io/LevelFile.h"
#include "util/HashUtil.h"

class TilePlacement;

namespace generator { namespace alpha {

struct DungeonSelectorState {
	const CaseInsensitiveId& inputId;
	const io::Teleport& teleport;
	Random& rnd;
};

struct DungeonBatchSelectorState {
	const TArray<TeleportDoorDef>& teleportDoorDefs;
	Random& rnd;
};

class GenState;
using StretchCompletion = std::function<bool(const GenState&)>;
using StretchTileProvider = std::function<TOptional<MetaTile>(const GenState&)>;
using DungeonSelector = std::function<TOptional<CaseInsensitiveId>(DungeonSelectorState)>;
using DungeonBatchSelector = std::function<std::map<int, CaseInsensitiveId>(DungeonBatchSelectorState)>;

namespace tilegetters {
const StretchTileProvider& None();
      StretchTileProvider  Default(MetaTileVector);
}

struct StretchData {
	io::Stretch stretch;

	StretchCompletion isDone;
	StretchTileProvider tileProvider;
	MetaTileVector deadEnds;

	DungeonSelector dungeonSelector;
	DungeonBatchSelector dungeonBatchSelector;
};

struct Env {
	const TileGroup& tileGroup;
	const std::vector<io::Dungeon>& dungeons;

	io::LevelGenConfig config;
	DungeonSelector dungeonSelector;
	DungeonBatchSelector dungeonBatchSelector;
};

//
// Generator Stuck types
//
struct GsStretch {
	GsStretch(const io::Stretch&);

	int index;
	int dungeonIndex;
	std::string id;
};

struct GsNoTile {
	GsNoTile(const io::Stretch&);
	bool operator==(const GsNoTile&) const;
	std::size_t hashCode() const;

	GsStretch stretch;
};

struct GsNoPlacement {
	enum class Type { NoCandidates, IntersectsGeometry };

	GsNoPlacement(Type, const io::Stretch&, const TOptional<TilePlacement>& previous, MetaTile);
	bool operator==(const GsNoPlacement&) const;
	std::size_t hashCode() const;

	std::string toString() const;

	Type type;
	GsStretch stretch;
	TOptional<std::string> previousTileName;
	MetaTile tile;
};

struct GsFailedPlacement {
	TOptional<GsNoTile> noTile;
	TOptional<GsNoPlacement> noPlacement;
};

}}

REDIRECT_HASH(generator::alpha::GsNoTile);
REDIRECT_HASH(generator::alpha::GsNoPlacement);

namespace generator { namespace alpha {

struct GeneratorStuck {
	void add(const GsNoTile&);
	void add(const GsNoPlacement&);
	void append(const GeneratorStuck&);

	// convenience helper
	void add(const GsFailedPlacement&);

	size_t errorCount() const;
	bool isEmpty() const;

	std::unordered_map<GsNoTile, size_t> noTiles;
	std::unordered_map<GsNoPlacement, size_t> noPlacements;
};

struct GenerationError {
	void append(const GenerationError&);

	GeneratorStuck stuck;
	ValidationIssues issues;
};

// Yes, I know what Either (and Maybe, under) usually are but the names work ok here.
template <typename SuccessRaw, typename SuccessParam, typename FailureRaw, typename FailureParam>
struct EitherRaw {
	EitherRaw(SuccessParam success) : success(std::move(success)) {}
	EitherRaw(FailureParam failed) : failed(std::move(failed)) {}
	SuccessRaw success;
	FailureRaw failed;
};

template <typename Success, typename Failure>
using Either = EitherRaw<TOptional<Success>, Success, TOptional<Failure>, Failure>;

//
// State and State "runner" (helper for updating the state values)
//
class GenState {
public:
	GenState(const Env&, const std::vector<io::Stretch>&, RandomSeed);

	const StretchData& stretchDef() const;
	const StretchData* stretchDefForTileIndex(int tileIndex) const;

	const io::Dungeon* dungeonForTileIndex(int tileIndex) const;

	void place(PlaceResult, StretchId);
	void placeOffsite(TOptional<DoorPair> from, const GenState&);

	Result makeResult() const;

	Env env;
	std::vector<StretchData> stretches;

	std::vector<StretchId> stretchIds;
	PlacedTiles placed;
	VolumesIntersector intersector;

	int currentStretchIndex = 0;
	int globalTileIndex = 0;

	int stretchIndex = 0;
	int stretchCount;
	std::vector<int> stretchLengths;

	int mainPathLength = 0;

	ValidationIssues issues;

	mutable LevelGenRandom rnd;

	int currentDungeonInstanceIndex = MainDungeonInstanceId - 1;
};

using MaybeGenResult = EitherRaw<Unique<GenState>, Unique<GenState>, TOptional<GenerationError>, GenerationError>;

class GenStateRunner {
public:
	GenStateRunner(GenState&);

	bool isDone() const;

	void push(PlaceResult);
	void pop(int count = 1);

	void goToNextStretch();
	void goToPreviousStretch();

	void setMainPathDone() const;
	
	void printStats() const;
private:
	GenState& s;
	int pushs = 0, pops = 0, nexts = 0, prevs = 0;
};

}}
