#pragma once

#include "CommonTypes.h"
#include "GameSettings.h"
#include "game/level/GameLevelDef.h"
#include "game/level/GameTiles.h"
#include "affector/Affectors.h"
#include "mission/state/MissionState.h"
#include "objective/RegionFinder.h"
#include <Array.h>

class AActor;
class UWorld;
class APlayerCharacter;
class AMobCharacter;
class AStaticToInstancedMeshConverter;
class UBehaviorSystem;
struct MissionDef;
struct FStreamableHandle;

namespace generator {
struct Stretch;
}

namespace analytics {
	class Analytics;
}

namespace game {
class GameProgress;
class Environment;

namespace ambience {
class AmbienceTracker;
}

namespace tile {
class TilePreparer;
}

namespace mobspawn {
class HyperSpawner;
namespace eventmob {
class EventMobSpawner;
}}

namespace objective {
class ObjectivesSystem;
class ChallengeSystem;
}

namespace events {
class TileEvents;
class TileEventDispatcher;
}

namespace trigger {
class Triggers;
}

namespace sublevel {
class TileLoader;
}

class Game {
public:
	Game(UWorld&, const LevelDef&, const FMissionState&, bool isServer);
	~Game();

	UWorld& world() const;
	const Tiles& tiles() const;
	const RegionFinder& regionFinder() const;
	const GameProgress& progress() const;
	//D11.PS had to add the game namespace as event conflicts with keyword
	events::TileEvents& tileEvents() const;
	const objective::ObjectivesSystem* objectives() const;
	const objective::ChallengeSystem* challenges() const;	
	trigger::Triggers& triggers() const;
	const Settings& settings() const;
	UBehaviorSystem* behaviorSystem();

	const MissionDef& missionDef() const;

	void update();
	bool Init();
	bool IsLobbyLevel() const;
	bool isCompleted() const;
	bool canEnterNight() const;
	void setAllowClientConsoleCommands(bool allow) { mAllowClientConsoleCommands = allow; }
	bool IsInitDone() const;

	void addPlayer(APlayerCharacter& player, bool isDropIn, bool isBeginningOfGame);
	void suicide();
	void forceEndGame() const;
	void forceCompleteCurrentObjective() const;
	const TArray<TWeakObjectPtr<APlayerCharacter>>& getPlayers() const { return mPlayers; }

	Unique<Environment> mEnvironment;
	const std::string& levelName() const;
	const std::string& roundId() const { return mRoundIdString; }

	const affector::Affectors& affectors() const;

	bool ClientConsoleCommandsAllowed() { return mAllowClientConsoleCommands; }
	bool LoadAsyncAndPollIntroCinematic() const;
	void StartIntroCinematic() const;

	void SetReplicatedAffectors(const FReplicatableAffectorsRules&);
	
	void SetNewEmergentDifficulty(FDifficulty increasedEmergentDifficulty);

	void updateAmbience() const;

	bool HasLoadedSublevels() const;

	const mobspawn::HyperSpawner* hyperSpawner() const { return mHyperSpawner.get(); }
	
private:	
	void setupTriggers() const;	
	void setupNavMesh();

	void revive(APlayerCharacter*) const;

	void updatePlayers();
	void updatePlayerCount();

	void initPlayerCharacterInLevel(APlayerCharacter& player, bool isDropIn, bool isBeginningOfGame) const;

	UWorld& mWorld;
	LevelDef mLevelDef;
	Settings mSettings;
	FMissionState mMissionState;
	bool mIsServer;

	Tiles mTiles;
	Unique<sublevel::TileLoader> mSublevelLoader;
	Unique<AStaticToInstancedMeshConverter> mStaticMeshConverter;
	Unique<ambience::AmbienceTracker> mAmbienceTracker;
	RegionFinder mRegionFinder;
	
	//D11.PS had to add game namespace to event. conflicts on XB1
	Unique<events::TileEventDispatcher> mTileEventDispatcher;
	Unique<trigger::Triggers> mTriggers;
	Unique<GameProgress> mProgress;
	affector::Affectors mAffectors;

	// This group is only run on the server (until we add e.g a ServerGame class)
	Unique<UBehaviorSystem> mBehaviorSystem;
	Unique<objective::ObjectivesSystem> mObjectives;
	Unique<objective::ChallengeSystem> mChallenges;
	Unique<mobspawn::HyperSpawner> mHyperSpawner;
	Unique<tile::TilePreparer> mTilePreparer;
	Unique<mobspawn::eventmob::EventMobSpawner> mEventMobSpawner;

	FGuid mRoundId;
	std::string mRoundIdString;

	TArray<TWeakObjectPtr<APlayerCharacter>> mPlayers;

	bool mAllowClientConsoleCommands;
	bool mCachedRoundId;

	int mInitState;
	TSharedPtr<FStreamableHandle> mPreloadTeleportDoorsHandle;
};
}
