#include "Dungeons.h"
#include "MobSpawnConfigs.h"
#include "MobAction.h"
#include "EntityTypeMappers.h"
#include "game/Game.h"
#include "game/mobspawn/internal/InternalPositionCorrectors.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "game/util/ActorQuery.h"
#include "util/SharedRandom.h"

namespace game { namespace mobspawn { namespace positioncorrectors {

bool IsReachable(UWorld& world, FVector location) {
	const auto terrain = UActorQuery::GetGameBP(&world)->GetTerrain();
	return terrain->getType(conversion::ueToTerrain(location)).isReachable();
}

const PositionCorrector& ByLevelTrace(bool ignorePlayers) {
	static const PositionCorrector playerCollisionIgnore = [](UWorld& world, EntityType type, FTransform transform) {
		return IsReachable(world, transform.GetLocation())
			? internal::correctPositionByLevelTrace(world, transform, true)
			: TOptional<FTransform> {};
	};

	static const PositionCorrector playerCollisionFail = [](UWorld& world, EntityType type, FTransform transform) {
		return IsReachable(world, transform.GetLocation())
			? internal::correctPositionByLevelTrace(world, transform, false)
			: TOptional<FTransform> {};
	};

	return ignorePlayers ? playerCollisionIgnore : playerCollisionFail;
}

PositionCorrector ByTileData(const Game& game) {
	return [&game](UWorld& world, EntityType type, FTransform transform) {
		return internal::correctPositionByTileData(game, type, transform);
	};
}

}

ConfigBuilder::ConfigBuilder()
	: rnd(&Util::sharedRandom()) {
}

//
// Config builder for the mob spawning
//
ConfigBuilder& ConfigBuilder::TypeMapper(EntityTypeMapper mapper) {
	config.typeMapper = std::move(mapper); return *this;
}

ConfigBuilder& ConfigBuilder::NoVariants() {
	return TypeMapper(mobspawn::NoVariants());
}

ConfigBuilder& ConfigBuilder::DefaultVariants(const FDifficulty& difficulty) {
	return TypeMapper(mobspawn::DefaultVariants(difficulty, rnd));
}

ConfigBuilder& ConfigBuilder::CorrectPositionBy(PositionCorrector corrector) {
	config.positionCorrector = std::move(corrector);
	return *this;
}

ConfigBuilder& ConfigBuilder::AdjustPositionIfPossible(bool value) {
	config.adjustPositionIfPossible = value;
	return *this;
}

ConfigBuilder& ConfigBuilder::Action(MobAction action) {
	config.actions.push_back(std::move(action));
	return *this;
}

ConfigBuilder& ConfigBuilder::PostSpawnAction(MobAction action) {
	config.postSpawnActions.push_back(std::move(action));
	return *this;
}

ConfigBuilder& ConfigBuilder::Actions(std::initializer_list<MobAction> list) {
	config.actions.insert(config.actions.end(), std::move(list));
	return *this;
}

ConfigBuilder& ConfigBuilder::AddTag(const FName& tag) {
	return Action(mobspawn::AddTag(tag));
}

ConfigBuilder& ConfigBuilder::Once() { return Retry(0); }
ConfigBuilder& ConfigBuilder::Retry(int retries /*= 9*/) {
	config.tries = retries + 1;
	return *this;
}

ConfigBuilder& ConfigBuilder::UseRandom(Random& random) {
	rnd = &random;
	return *this;
}

ConfigBuilder& ConfigBuilder::ApplyDifficultyEffects() {
	PostSpawnAction([](AMobCharacter& mob) {
		if (auto* gameMode = Cast<ADungeonsGameMode>(mob.GetWorld()->GetAuthGameMode())) {
			gameMode->ApplyDifficultyEffects(mob);
		}
	});
	return *this;
}

ConfigBuilder& ConfigBuilder::ApplyPlayerDifficultyEffects() {
	PostSpawnAction([](AMobCharacter& mob) {
		if (auto* gameMode = Cast<ADungeonsGameMode>(mob.GetWorld()->GetAuthGameMode())) {
			gameMode->ApplyPlayerDifficultyEffects(mob);
		}
	});
	return *this;
}

ConfigBuilder& ConfigBuilder::SpawnAsUnderling() {
	return Action(mobspawn::SpawnAsUnderling());
}

ConfigBuilder& ConfigBuilder::ChangeMaster(ABaseCharacter* master) {
	return Action(mobspawn::ChangeMaster(master));
}

ConfigBuilder& ConfigBuilder::ChangeTeam(ETeamName newTeam) {
	return Action(mobspawn::ChangeTeam(newTeam));
}

ConfigBuilder& ConfigBuilder::SetTeleportToOwner(ABaseCharacter* owner) {
	return PostSpawnAction(mobspawn::SetTeleportToOwner(owner));
}

ConfigBuilder::operator Config() const {
	return config;
}

namespace configs {

ConfigBuilder Default(const FDifficulty& difficulty, bool ignorePlayers) {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByLevelTrace(ignorePlayers))
		.DefaultVariants(difficulty)
		.ApplyDifficultyEffects()
		;
}

ConfigBuilder DefaultNoVariants(bool ignorePlayers) {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByLevelTrace(ignorePlayers))
		.NoVariants()
		.ApplyDifficultyEffects()
		;
}

game::mobspawn::ConfigBuilder DefaultNoVariantsNoDifficulty(bool ignorePlayers) {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByLevelTrace(ignorePlayers))
		.NoVariants()
		;
}

ConfigBuilder DefaultNoAdjustPosition(const FDifficulty& difficulty, bool ignorePlayers) {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByLevelTrace(ignorePlayers))
		.AdjustPositionIfPossible(false)
		.DefaultVariants(difficulty)
		.ApplyDifficultyEffects()
		;
}

game::mobspawn::ConfigBuilder PlayerPets() {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByLevelTrace(true))
		.NoVariants()
		.ApplyPlayerDifficultyEffects()
		;
}

ConfigBuilder DefaultTileData(const Game& game) {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByTileData(game))
		.DefaultVariants(game.settings().difficulty)
		.ApplyDifficultyEffects()
		;
}

game::mobspawn::ConfigBuilder DefaultTileDataNoVariants(const Game& game) {
	return ConfigBuilder()
		.CorrectPositionBy(positioncorrectors::ByTileData(game))
		.NoVariants()
		.ApplyDifficultyEffects()
		;
}

}

}}
