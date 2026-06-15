#include "Dungeons.h"
#include "DamageHelpers.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/level/GameTiles.h"
#include "world/level/BlockPos.h"
#include "util/Algo.h"

#include <GameFramework/Actor.h>

namespace damagehelpers {

bool tryKillPlayerByKillZone(APlayerCharacter& player, bool isFreezeZone) {
	if (!player.GetController()) {
		// Weird state where _initial spawn_ (i.e. spawned by UE itself) is inside a death region
		return false;
	}
	if (player.GetWorldState() != ECharacterWorldState::InWorld) {
		return false;
	}
	player.GetController()->StopMovement();
	player.SetWorldState(ECharacterWorldState::DisappearedWithCollision);

	if (isFreezeZone) {
		FTimerHandle reviveHandle;
		player.GetWorld()->GetTimerManager().SetTimer(
			reviveHandle,
			FTimerDelegate::CreateUObject(&player, &APlayerCharacter::RespawnFrozen),
			0.75f,
			false
		);

		// #D11.CM - We've entered a freeze region, so play the splash effect.
		player.OnPlayerEnterFreezingWater.Broadcast();
	}
	else {
		FTimerHandle reviveHandle;
		player.GetWorld()->GetTimerManager().SetTimer(
			reviveHandle,
			FTimerDelegate::CreateUObject(&player, &APlayerCharacter::RespawnQuick),
			1.5f,
			false
		);
	}
	return true;
}

bool tryKillByKillZone(AActor& actor, bool isFreezeZone) {
	if (auto character = Cast<APlayerCharacter>(&actor)) {
		return tryKillPlayerByKillZone(*character, isFreezeZone);
	}
	if (auto healthComponent = actor.template FindComponentByClass<UHealthComponent>()) {
		healthComponent->KillWith(damageTag::killzone());
		return true;
	}
	return false;
}

ECharacterBoundsCheckPenalty evaluateBoundsCheckPenalty(const game::Tiles& tiles, const BlockPos& pos) {
	auto tile = tiles.getClosestTile(pos);
	if (!tile || tile->bounds().containsXZ(pos)) {
		// !tile means level has no tiles. This shouldn't happen. Trying to figure out
		// if we should (or already are) stopping this somewhere else @todo @notiles
		return ECharacterBoundsCheckPenalty::Nothing;
	}
	if (tile->dungeon().def().deathOutsideTile) {
		return ECharacterBoundsCheckPenalty::KillZoneLike;
	}
	if (pos.y < algo::opt::get_or_compute(tile->tilePlacement().lowestWalkableBlockY(), RETLAMBDA0(tiles.getSubDungeonInfo(tile->dungeon()).bounds.minInclusive.y))) {
		return ECharacterBoundsCheckPenalty::KillZoneLike;
	}
	return ECharacterBoundsCheckPenalty::Nothing;
}

}
