#include "Dungeons.h"
#include "TileQuery.h"
#include "ActorQuery.h"
#include "game/Conversion.h"
#include "game/Game.h"
#include <Engine/Engine.h>

namespace tilequeryinternal {

//
// TileRotation and factories/helpers
//

const AActor* getActorFromWorldContext(const UObject* worldContextObject) {
	if (const auto* actor = Cast<AActor>(worldContextObject)) {
		return actor;
	}
	if (auto* component = Cast<UActorComponent>(worldContextObject)) {
		return component->GetOwner();
	}
	return nullptr;
}

game::TilePtr getTile(UWorld* world, FVector location) {
	if (auto* game = actorquery::getGame(world)) {
		return game->tiles().getTile(conversion::ueToBlock(location));
	}
	return nullptr;
}

game::TilePtr getClosestTile(UWorld* world, FVector location) {
	if (auto* game = actorquery::getGame(world)) {
		return game->tiles().getClosestTile(conversion::ueToBlock(location));
	}
	return nullptr;
}

namespace degrees {

TOptional<FTileRotation> getTileDegrees(UWorld* world, FVector location) {
	if (const auto* tile = getTile(world, location)) {
		return TileRotationFromJsonDegrees(quadrantToDegrees(tile->tilePlacement().originalPlacement()->placement().rotation));
	}
	return {};
}

}

namespace resourcepack {

static const EResourcePack Default = EResourcePack::Vanilla;

TOptional<EResourcePack> getResourcePack(UWorld* world, FVector location) {
	if (const auto* tile = getClosestTile(world, location)) {
		return tile->dungeon().resourcePack();
	}
	return {};
}

}

namespace ambience {

TOptional<FString> getTileGroupName(UWorld* world, FVector location) {
	if (const auto* tile = getClosestTile(world, location)) {
		return tile->ambienceGroupName();
	}
	return {};
}

}
}

FTileRotation TileRotationFromUeDegrees(float degrees) {
	return FTileRotation {degrees, conversion::ueToDegrees(degrees) };
}

FTileRotation TileRotationFromJsonDegrees(float degrees) {
	return TileRotationFromUeDegrees(conversion::degreesToUe(degrees));
}

FTileRotation TileRotationWithFailState(float stateDegrees) {
	return FTileRotation{ stateDegrees, stateDegrees };
}

//
// Tile Degrees
//
FTileRotation UTileQuery::GetTileDegrees(const AActor* actor) {
	return tilequeryinternal::degrees::getTileDegrees(actor->GetWorld(), actor->GetActorLocation()).Get(TileRotationWithFailState(-360));
}

FTileRotation UTileQuery::GetTileDegreesAtLocation(UObject* WorldContextObject, FVector AtLocation) {
	return tilequeryinternal::degrees::getTileDegrees(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull), AtLocation).Get(TileRotationWithFailState(-360));
}

FTileRotation UTileQuery::GetMyTileDegrees(UObject* WorldContextObject) {
	if (const auto* actor = tilequeryinternal::getActorFromWorldContext(WorldContextObject)) {
		return GetTileDegrees(actor);
	}
	ensure(false && "GetMyTileDegrees called from an object without world location?");
	return TileRotationWithFailState(-720);
}

//
// ResourcePacks
//
EResourcePack UTileQuery::GetResourcePackAtLocation(const UObject* WorldContextObject, FVector AtLocation) {
	return tilequeryinternal::resourcepack::getResourcePack(WorldContextObject->GetWorld(), AtLocation).Get(tilequeryinternal::resourcepack::Default);
}

EResourcePack UTileQuery::GetResourcePack(const AActor* actor) {
	return GetResourcePackAtLocation(actor, actor->GetActorLocation());
}

EResourcePack UTileQuery::GetMyTileResourcePack(const UObject* WorldContextObject) {
	if (const auto* actor = tilequeryinternal::getActorFromWorldContext(WorldContextObject)) {
		return GetResourcePack(actor);
	}
	return tilequeryinternal::resourcepack::Default;
}

//
// Ambience GroupName
//
FString UTileQuery::GetMyTileAmbienceGroupName(const UObject* WorldContextObject) {
	if (const auto* actor = tilequeryinternal::getActorFromWorldContext(WorldContextObject)) {
		return tilequeryinternal::ambience::getTileGroupName(WorldContextObject->GetWorld(), actor->GetActorLocation()).Get({});
	}
	return {};
}
