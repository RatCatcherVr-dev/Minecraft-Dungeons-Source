// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "Dungeons.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/RaidCaptain/RaidCaptainSpawner.h"
#include "lovika/RegionPredicates.h"
#include "game/mission/request/MissionRequestUtil.h"
 
namespace game 
{
namespace mobspawn 
{
	RaidCaptainSpawner::RaidCaptainSpawner(UWorld& world, Config spawnConfig, const FMissionState& missionState, const Tiles& tiles)
		: mWorld(&world)
		, mSpawnConfig(std::move(spawnConfig))
		, MissionState(missionState)
		, CurrentAmountOfSpawns(0)
		, MaxAmountOfSpawns(6)
		, MinAmountOfMobToSpawn(6)
		, MaxAmountOfMobToSpawn(12)
	{

		mTiles = algo::copy_if(tiles.getTiles(),
			[](const auto& tile) -> bool
			{
				return !tile->isObjectiveTileHACK() && tile->spawnRegions()
					&& (tile->dungeon().instanceId() != 1 || tile->progress().isOnStrayPath());
			}
		);

		Random rnd(tiles.getTiles().size());
		mTiles = algo::random::shuffledCopy(mTiles, rnd);

		if (mTiles.size() > MaxAmountOfSpawns)
		{
			mTiles.erase(mTiles.begin() + MaxAmountOfSpawns, mTiles.end());
		}
	}

	void RaidCaptainSpawner::operator()(tile::TilePreparationState state) noexcept
	{
		// Don't allow Raid Captain spawns if we're in an Ancient Hunt neither if we never completed this mission
		if (MissionState.missionDifficulty.mission == ELevelNames::netherhypermission || MissionState.missionDifficulty.mission == ELevelNames::thestronghold || !MissionState.bCompletedOnce)
		{
			return;
		}

		// Tile doesn't allow Raid Captains
		if (!state.tile.stretch().def.allowRaidCaptains) {
			return;
		}

		if (CurrentAmountOfSpawns == MaxAmountOfSpawns
			|| CurrentAmountOfSpawns == mTiles.size())
		{
			return;
		}


		if (algo::contains(mTiles, &state.tile)) 
		{
			auto group = SpawnGroup(
				state.rnd.nextBoolean() ?
				EntityType::VindicatorRaidCaptain : EntityType::PillagerRaidCaptain
				, 1);

			const auto transformProvider = providers::FromProviders(
				providers::position::Regions(state.tile.spawnRegions()));

			AGameBP* game = actorquery::getFirstActor<AGameBP>(mWorld.Get());

			game->RequestMobGroupSpawn(group, transformProvider, mSpawnConfig);

			int countCapped = state.rnd.nextInt(MinAmountOfMobToSpawn, MaxAmountOfMobToSpawn);

			io::MobGroup mobGroupToSpawn = createMobGroup();
			game::mobspawn::prepareMobGroup(mobGroupToSpawn);
			for (auto entityType : game::mobspawn::calculateMobsWithoutDifficultyLimitation({ mobGroupToSpawn }, countCapped))
			{
				UClass* pMobClass = nullptr;
				FTransform MobTransform;
				if (!game::mobspawn::getSpawnData(*mWorld, transformProvider, mSpawnConfig, entityType, MobTransform))
				{
					continue;
				}
				game->RequestMobSpawn(entityType, MobTransform, {}, mSpawnConfig);
			}
		}
	}

	io::MobGroup RaidCaptainSpawner::createMobGroup() {
		static const std::map<int, std::vector<EntityType>> mobGroups{
			{
				1,
				{EntityType::Vindicator, EntityType::Pillager}
			},
			{
				2,
				{EntityType::Vindicator, EntityType::Pillager, EntityType::Enchanter,}
			},
			{
				3,
				{EntityType::Enchanter, EntityType::Witch, EntityType::Geomancer}
			},
			{
				4,
				{EntityType::Vindicator, EntityType::RoyalGuard}
			},
			{
				5,
				{EntityType::Vindicator, EntityType::Chillager}
			},
			{
				6,
				{EntityType::Pillager, EntityType::Mountaineer}
			},
			{
				7,
				{EntityType::Vindicator, EntityType::Enchanter, EntityType::WindCaller}
			},
		};

		static const std::map<int, std::initializer_list<float>> probabilities
		{
			{
				1,
				std::initializer_list<float>{0.7f, 0.3f}
			},
			{
				2,
				std::initializer_list<float>{0.2f, 0.6f, 0.2f}
			},
			{
				3,
				std::initializer_list<float>{0.2f, 0.4f, 0.4f}
			},
			{
				4,
				std::initializer_list<float>{0.8f, 0.2f}
			},
			{
				5,
				std::initializer_list<float>{0.7f, 0.3f}
			},
			{
				6,
				std::initializer_list<float>{0.3f, 0.7f}
			},
			{
				7,
				std::initializer_list<float>{0.6f, 0.2f, 0.2f}
			}
		};

		int randomGroupNum = FMath::RandRange(1, mobGroups.size());

		return game::mobspawn::sampleWithProbabilities(mobGroups.at(randomGroupNum), probabilities.at(randomGroupNum));
	}
}
}