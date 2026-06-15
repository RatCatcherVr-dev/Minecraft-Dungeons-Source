#include "Dungeons.h"
#include "AmbienceTracker.h"
#include "AmbienceVolume.h"
#include "game/Conversion.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/level/ambience/AmbienceUtil.h"
#include "game/level/GameTiles.h"
#include "game/util/ActorQuery.h"
#include "game/actor/MissionLootCameraActor.h"
#include "Model.h"

namespace game { namespace ambience {

AmbienceTracker::AmbienceTracker(UWorld& world, const Tiles& tiles)
	: mWorld(world)
	, mTiles(tiles) {
}

EAmbienceAudioID AmbienceTracker::getAmbienceAudioFor(const AAmbienceVolume* ambienceVolume, const Tile* tile) const {
	if (ambienceVolume) {
		if (ambienceVolume->GetAmbienceAudio())
			return ambienceVolume->GetAmbienceAudio().GetValue();
	}

	if (tile) {
		if (const auto tileAudio = tile->ambienceAudio()) {
			return tileAudio.GetValue();
		}
	}
	return EAmbienceAudioID::AUDIO_FROM_AMBIENCE;
}

EAmbienceID AmbienceTracker::getAmbienceFor(const AAmbienceVolume* ambienceVolume, const Tile* tile) const {
	if (actorquery::getFirstActor<AMissionLootCameraActor>(&mWorld)) {
		return EAmbienceID::AMBIENCE_ALPHA;
	}
	if (ambienceVolume) {
		if (ambienceVolume->GetAmbience())
			return ambienceVolume->GetAmbience().GetValue();
	}
	if (tile) {
		if (const auto tileAmbience = tile->ambience()) {
			return tileAmbience.GetValue();
		}
	}
	return EAmbienceID::AMBIENCE_DEFAULT;
}

AAmbienceVolume* AmbienceTracker::getAmbienceVolumeAt(const FVector& location) const {
	const auto volumeMustMatchLocation = [=](const AAmbienceVolume* volume) {
		return volume->EncompassesPoint(location);
	};

	auto ambienceVolumes = actorquery::getActors<AAmbienceVolume>(&mWorld);

#if !UE_BUILD_SHIPPING
	for (const AAmbienceVolume* volume : ambienceVolumes) {
		int numVertices = volume->Brush->Verts.Num();
		ensureMsgf(numVertices < 200, TEXT("Unreasonable number of vertices (%d) in an AmbienceVolume (%s). Consider changing to a simpler shape."), numVertices, *volume->GetName());
	}
#endif

	if (const auto matchingVolume = ambienceVolumes.FindByPredicate(volumeMustMatchLocation)) {
		return *matchingVolume;
	}

	return nullptr;
}

bool AmbienceTracker::isAnyLocalPlayerInInventory() const {
	for (auto player : InstanceTracker< APlayerCharacter >::GetList(&mWorld)) {
		if (player->IsLocallyControlled() && player->IsInInventory()) {
			return true;
		}
	}
	return false;
}

void AmbienceTracker::updatePlayerAmbience() const {
	//Players update their ambience
	for (auto* player : InstanceTracker<APlayerCharacter>::GetList(&mWorld)) {
		const auto* closestTile = mTiles.getClosestTile(*player);
		const FString ambienceGroup = closestTile ? closestTile->ambienceGroupName() : "";
		updateAmbienceFor(player, ambienceGroup);
	}

	TOptional<FAmbienceIDGroup> mostRelevantAmbience;
	TOptional<FAmbienceAudioIDGroup> mostRelevantAudioAmbience;

	for (const auto* player : InstanceTracker<APlayerCharacter>::GetList(&mWorld)) {
		if (player->IsLocallyControlled()) {			
			//First local player is the ambience which is activated.
			mostRelevantAudioAmbience = player->GetAmbienceAudio();
			mostRelevantAmbience = player->GetAmbience();
			break;
		}
	}

	if (mostRelevantAmbience) {
		AmbienceUtil::SetActiveAmbienceT<AAmbienceAudioActor>(&mWorld, mostRelevantAudioAmbience.GetValue());

		if (AAmbienceVisualActor* ActiveAmbience = AmbienceUtil::SetActiveAmbienceT<AAmbienceVisualActor>(&mWorld, mostRelevantAmbience.GetValue())) {
			ActiveAmbience->SetHidden(isAnyLocalPlayerInInventory());
		}
	}
}

void AmbienceTracker::updateAmbienceFor(APlayerCharacter* player, const FString& ambienceGroup) const {
	using namespace game::ambience;
	
	AAmbienceVolume* ambienceVolume = getAmbienceVolumeAt(player->GetActorLocation());
	const Tile* tile = mTiles.getTile(conversion::ueToBlock(player->GetActorLocation()));

	{
		// update for visuals
		const FAmbienceIDGroup currentAmbience(getAmbienceFor(ambienceVolume, tile), ambienceGroup);
		const FAmbienceIDGroup oldAmbience = player->GetAmbience();
		if (currentAmbience != oldAmbience) {
			AmbienceUtil::PlayerExitedAmbienceT<AAmbienceVisualActor>(player, oldAmbience);			
			AmbienceUtil::PlayerEnteredAmbienceT<AAmbienceVisualActor>(player, currentAmbience);
			player->SetAmbience(currentAmbience);
		}
	}

	{
		// update for audio
		const FAmbienceAudioIDGroup currentAudio(getAmbienceAudioFor(ambienceVolume, tile), ambienceGroup);
		const FAmbienceAudioIDGroup oldAudioAmbience = player->GetAmbienceAudio();
		if (currentAudio != oldAudioAmbience) {
			AmbienceUtil::PlayerExitedAmbienceT<AAmbienceAudioActor>(player, oldAudioAmbience);
			AmbienceUtil::PlayerEnteredAmbienceT<AAmbienceAudioActor>(player, currentAudio);
			player->SetAmbienceAudio(currentAudio);
		}
	}
}

}}
