#include "Dungeons.h"
#include "Decorator.h"
#include "lovika/world/level/levelgen/LevelDef.h"

void decorator::decorate(levelgen::LevelDef& levelDef, const decorator::TileDecorator& decorator, RandomSeed seed) {
	for (generator::Tile& tile : levelDef.tiles) {
		auto& placement = tile.placeResult.tilePlacement;
		auto& stretch = levelDef.stretches[tile.stretchId.index];

		static LevelGenRandom rnd;
		rnd.setSeed(seed ^ placement.bounds().maxExclusive.hashCode());

		decorator::State decoratorState{
			placement,
			placement.children(),
			rnd,
			tile.overrides(levelDef).propDensity.Get(0.25f),
			stretch.def.id,
			stretch.def.propGroups // @stretchid
		};
		decorator.decorate(decoratorState);
	}
}
