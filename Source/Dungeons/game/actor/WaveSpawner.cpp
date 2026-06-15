#include "WaveSpawner.h"
#include "character/player/PlayerCharacter.h"
#include <NavigationSystem.h>
#include "game/component/PlayerExperienceComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "EffectsActor.h"
#include "game/GameBP.h"
#include "game/component/DistancelimitComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/mobspawn/MobAction.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/util/ActorQuery.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/util/LocationQuery.h"
#include "game/util/Tags.h"


AWaveSpawner::AWaveSpawner(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .25f;
}

void AWaveSpawner::BeginPlay()
{
	Super::BeginPlay();

	mobGroup = {};

	game = actorquery::getFirstActor<AGameBP>(GetWorld());
	if (game.IsValid() && game.Get()->GetGame() == nullptr) {
		game.Get()->OnGameSettingsInitiallized.AddDynamic(this, &AWaveSpawner::OnGameReady);
	}
	else
	{
		OnGameReady();
	}
}

void AWaveSpawner::Despawn(AMobCharacter* mob)
{
	if (mob == nullptr)
	{
		return;
	}

	const auto effects = AEffectsActor::GetInstance(GetWorld());

	effects->SpawnEffectsAtLocation(cleanupParticles, cleanupSound, mob->GetActorLocation() + UDungeonsEffectLibrary::FindFeetOffsetForActor(mob));
	if (despawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, despawnSound, mob->GetActorLocation());
	}

	mob->SetActorHiddenInGame(true);
	mob->SetLifeSpan(0.001f);
}

void AWaveSpawner::OnGameReady()
{
	EDLCName DLCName = EDLCName::Invalid;
	if (auto levelGame = actorquery::getGame(GetWorld())) {
		if (levelGame->settings().levelName != ELevelNames::Invalid)
		{
			const MissionDef& mission = missions::get(levelGame->settings().levelName);
			if (mission.getRequiredDLC().IsSet())
			{
				DLCName = mission.getRequiredDLC().GetValue();
			}
		}
	}

	TArray<FWeightedMobType> mobTypes = weightedMobTypes;
	if (DLCName != EDLCName::Invalid)
	{
		FDLCWeightedMobType* weightedMobType = dlcWeightedMobTypes.FindByPredicate([=](const FDLCWeightedMobType& WeightMobType) {
			return WeightMobType.DLCType == DLCName;
		});


		if (weightedMobType) {
			mobTypes = weightedMobType->MobTypes;
		}
	}

	for (const FWeightedMobType& elem : mobTypes)
	{
		const std::string typeName{ TCHAR_TO_UTF8(*elem.mobType) };
		mobGroup.types.push_back({ typeName, elem.weight });
	}
	game::mobspawn::prepareMobGroup(mobGroup);

	fraction = 0.f;


	if (game.IsValid()) {
		game->GetMobLoader()->AsyncLoadMobs(UMobLoaderComponent::ExtractEntitiesFromGroup(mobGroup));
	}
}

void AWaveSpawner::CalculateFraction(float DeltaTime)
{
	// Spawn rate scales with night fraction, the internal spawn rate increase and number of players alive.
	fraction += spawnRateMax * DeltaTime * NumPlayerAlive;
}

TOptional<FTransform> AWaveSpawner::GenerateTransform(UWorld* world, const TArray<APlayerCharacter*>& players)
{
	static Random rnd;
	const auto randomPlayer = players[rnd.nextInt(players.Num())];

	AActor* CenterActor = GetActorLocation().IsNearlyZero() ? randomPlayer : Cast<AActor>(this);
	const auto center = GetSpawnCentrePoint(CenterActor);

	const auto randomLocationAround = [&]() -> TOptional<FVector> {
		FNavLocation result;
		const auto nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
		const bool found = nav->GetRandomReachablePointInRadius(center, spawnRadius, result);

		if (found)
		{
			return {result.Location};
		}

		return {};
	};

	const auto isReachableAndFar = [&](const FVector& location) {
		for (const auto player : players)
		{
			if (FVector::DistSquared2D(player->GetActorLocation(), location) < spawnMinDistance * spawnMinDistance)
			{
				return false;
			}
		}

		return randomPlayer->IsLocationReachable(location);
	};

	const auto maybeLocation = locationquery::retry(randomLocationAround, isReachableAndFar, 5);

	if (maybeLocation.IsSet())
	{
		const auto location = maybeLocation.GetValue();
		const auto delta = randomPlayer->GetActorLocation() - location;

		return FTransform{{0.f, FMath::RadiansToDegrees(FMath::Atan2(delta.Y, delta.X)), 0.f}, location};
	}

	return {};
}

FVector AWaveSpawner::GetSpawnCentrePoint(AActor*) const
{
	return GetActorLocation();
}

void AWaveSpawner::Tick(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	Super::Tick(DeltaTime);
}

void AWaveSpawner::TickSpawner(float DeltaTime)
{
	const auto world = GetWorld();

	const auto playersAlive = actorquery::getActors<APlayerCharacter>(world).FilterByPredicate([](APlayerCharacter* player) { return player->IsAlive(); });

	NumPlayerAlive = playersAlive.Num();

	if (NumPlayerAlive <= 0)
	{
		return;
	}

	CalculateFraction(DeltaTime);

	// Wave size is scaled by number of players alive using a factor.
	const float ScaledWaveSize = debugScaledWaveSize = currentWaveSize * ((1.0f - waveSizePerPlayerScalingFactor) + waveSizePerPlayerScalingFactor * NumPlayerAlive);

	// Only spawn when fraction is ready to spawn a full wave
	if (fraction < ScaledWaveSize)
	{
		return;
	}

	const int count = FMath::FloorToInt(fraction);

	mobs.RemoveAll([](auto mob) { return mob == nullptr; });
	const int countCapped = debugSpawnRequestCount = FMath::Min3(countMax - mobs.Num() - requestedSpawnCount, count, (int)ScaledWaveSize);

	// For each wave - increase wave size up to max
	currentWaveSize = FMath::Min(waveSizeMax, currentWaveSize + waveSizeIncreasePerWave);

	if (countCapped <= 0)
	{
		fraction -= count;
		return;
	}

	if (!game.IsValid())
	{
		game = actorquery::getFirstActor<AGameBP>(world);
	}

	TWeakObjectPtr<AWaveSpawner> weak_this = this;

	for (auto entityType : game::mobspawn::calculateMobsWithoutDifficultyLimitation({ mobGroup }, countCapped))
	{
		if (const auto maybeTransform = locationquery::retry([&] { return GenerateTransform(world, playersAlive); }))
		{
			const auto& difficulty = game->GetGame() ? game->GetGame()->settings().difficulty : game::FDifficulty::DEFAULT;
			const auto spawnConfig = game::mobspawn::configs::Default(difficulty, false)
				.Actions({ game::mobspawn::AddTag(tags::noDrop), game::mobspawn::AddComponent<UDistancelimitComponent>() });

			FTransform MobTransform;

			if (!game::mobspawn::getSpawnData(*world, RETLAMBDA0(maybeTransform.GetValue()), spawnConfig, entityType, MobTransform))
			{
				continue;
			}

			++requestedSpawnCount;
			fraction -= 0.9f;

			// request a spawn with callback
			game->RequestMobSpawn(entityType, MobTransform, {}, spawnConfig, [weak_this](AMobCharacter* pMob) {
				if (weak_this.IsValid())
				{
					if (pMob)
					{
						pMob->SpawnEffectsAtLocation(weak_this->spawnParticles, weak_this->spawnSound, true);
						weak_this->mobs.Add(pMob);
						weak_this->OnSuccessfulMobSpawn.Broadcast();
					}
					else
					{
						weak_this->fraction += 0.9f;
					}

					weak_this->requestedSpawnCount--;
				}
			});
		}
		fraction -= 0.1f;  // minimum loss (subtracted even if nothing is spawned)
	}
}
