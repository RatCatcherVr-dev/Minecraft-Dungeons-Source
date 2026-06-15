#pragma once

#include "RegionFinder.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "lovika/Region.h"

class FOutputDevice;

namespace game {

class Game;
class Tile;
using TileRef = const Tile&;
	
namespace objective {

class Objective;

class ChallengeSystem {
public:
	ChallengeSystem(Game&, FOutputDevice* = nullptr);
	
	TArray<FVector> getRewardLocations() const;

	void tick();

	static const std::vector<FString> DEFAULT_REWARD_PREFABS;
	
private:
	void start();
	Game& game() const;

	struct Item {
		Item(RegionFinder, io::ObjectiveReward, Unique<Objective>);
		RegionFinder targetFinder; // @todo: extend this
		io::ObjectiveReward reward;
		Unique<Objective> objective;
		TOptional<lovika::Region> lootRegion;
	};
	void spawnReward(Item&);

	Game& mGame;
	FOutputDevice& mLog;
	TArray<TUniquePtr<Item>> mChallenges;
	int mTicksTilStart;
};

}}
