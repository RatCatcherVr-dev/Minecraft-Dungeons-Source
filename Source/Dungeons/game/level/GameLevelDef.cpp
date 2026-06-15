#include "Dungeons.h"
#include "GameLevelDef.h"
#include "util/Algo.h"

game::LevelDef::LevelDef(const levelgen::LevelDef& levelDef, RandomSeed finalSeed)
	: levelDef(levelDef)
	, finalSeed(finalSeed)
	, placedTiles(tilesToPlaceResults(levelDef.tiles))
{
	levelLength = algo::sum(levelDef.stretches, RETLAMBDA(it.length));
}
