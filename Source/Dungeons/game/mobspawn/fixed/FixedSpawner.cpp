#include "Dungeons.h"
#include "FixedSpawner.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "lovika/RegionPredicates.h"
#include "game/Conversion.h"
#include "game/level/GameTiles.h"
#include "game/GameBP.h"

namespace game { namespace mobspawn {

FixedSpawner::FixedSpawner(UWorld& world, Config spawnConfig)
	: mWorld(world)
	, mSpawnConfig(std::move(spawnConfig)) {
}

void FixedSpawner::operator()(tile::TilePreparationState state) {
	auto& tile = state.tile;

	AGameBP* game = actorquery::getFirstActor<AGameBP>(&mWorld);

	// D11.DB - Immobile regions for static mobs (eg. vines in DingyJungle).
	for (auto& region : tile.tilePlacement().filterRegions(regionpredicates::isFixedMob())) {
		auto name = region.name();
		if (name == "vine1") { name = "quickgrowingvine"; }
		if (name == "vine2") { name = "poisonquillvine"; }
		if (const auto type = MaybeEntityTypeFromString(name)) {
			for (const auto& block : region.area()) {
				auto group = SpawnGroup(type.GetValue(), 1);
				auto provider = providers::Location(conversion::blockCenterXZToUe(block));
				game->RequestMobGroupSpawn(group, provider, mSpawnConfig);
			}
		}
	}
}

}}
