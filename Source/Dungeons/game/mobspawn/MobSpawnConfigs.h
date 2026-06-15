#pragma once

#include "MobSpawnTypes.h"

class ABaseCharacter;
class FName;
class UWorld;
enum class ETeamName : uint8;

namespace game {

class Game;
	
namespace mobspawn {
	
namespace positioncorrectors {

const PositionCorrector& ByLevelTrace(bool ignorePlayers);
      PositionCorrector  ByTileData(const Game&);

}

struct ConfigBuilder {
	ConfigBuilder();

	// Entity type mapping (e.g. variants)
	ConfigBuilder& TypeMapper(EntityTypeMapper);
	ConfigBuilder& NoVariants();
	ConfigBuilder& DefaultVariants(const FDifficulty&);

	ConfigBuilder& Action(MobAction);
	ConfigBuilder& PostSpawnAction(MobAction);
	ConfigBuilder& Actions(std::initializer_list<MobAction>);
	ConfigBuilder& AddTag(const FName& tag);

	ConfigBuilder& CorrectPositionBy(PositionCorrector);
	ConfigBuilder& AdjustPositionIfPossible(bool);

	ConfigBuilder& Once();
	ConfigBuilder& Retry(int retries = 9);

	ConfigBuilder& UseRandom(Random&);

	ConfigBuilder& ApplyDifficultyEffects();
	ConfigBuilder& ApplyPlayerDifficultyEffects();
	ConfigBuilder& SpawnAsUnderling();
	
	ConfigBuilder& ChangeMaster(ABaseCharacter* master);
	ConfigBuilder& ChangeTeam(ETeamName newTeam);

	ConfigBuilder& SetTeleportToOwner(ABaseCharacter*);

	operator Config() const;
	Config config;
private:
	Random* rnd;
};

namespace configs {
	// These correct positions by level tracing
	ConfigBuilder Default(const FDifficulty&, bool ignorePlayers);
	ConfigBuilder DefaultNoVariants(bool ignorePlayers);
	ConfigBuilder DefaultNoVariantsNoDifficulty(bool ignorePlayers);
	ConfigBuilder DefaultNoAdjustPosition(const FDifficulty&, bool ignorePlayers);
	ConfigBuilder PlayerPets();

	// These correct positions by reading tile data (in a weird way)
	ConfigBuilder DefaultTileData(const Game&);
	ConfigBuilder DefaultTileDataNoVariants(const Game&);
}

}}

