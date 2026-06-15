#include "Dungeons.h"
#include "TileDecor.h"
#include "GameLevelDef.h"
#include "SubLevelTileLoader.h"
#include "game/Conversion.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/actor/character/loot/LobbyChest.h"
#include "game/level/ambience/AmbienceActor.h"
#include "game/util/ActorQuery.h"
#include "editor/decoration/DecorationAnchor.h"
#include "lovika/LovikaLevelActor.h"
#include "lovika/QuadrantAngle.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include <Engine.h>
#include <LogMacros.h>
#include <Components/ArrowComponent.h>
#include "util/StringUtil.h"

namespace decor {

FString subLevelContentRelativePath(const std::string& folderName) {
	return { ("Decor/Maps/" + folderName + "/SubLevels/").c_str() };
}

bool contentFolderExists(const FString& contentFolder) {
	return FPaths::DirectoryExists(FPaths::GameDir() + "Content/" + contentFolder);
}

FString filenameFor(const FString& group, const FString& tile) {
	auto basePath = FString("/Game/Decor/Generated/");
	auto rest = tile;

	if (group != FString("")) {
		rest = group + '/' + rest;
	}
	return basePath + rest.ToLower();
}

DUNGEONS_API FString filenameForActor(const FString& group, const FString& actor) {
	return filenameFor(group, "actor/" + actor);
}

DUNGEONS_API FString filenameForEnvironmental(const FString& group, const FString& actor) {
	return filenameFor(group, "light/" + actor);
}

DUNGEONS_API FString pathForSublevels(const FString& mission, const FString& group) {
	FString missionId = mission.ToLower();
	FString groupId = group.ToLower();

	if (missionId == groupId || groupId.IsEmpty()) {
		return "/Game/" + subLevelContentRelativePath(stringutil::toStdString(missionId));
	}

	FString missionSpecific = groupId + "_" + missionId;
	FString relativePath = subLevelContentRelativePath(stringutil::toStdString(missionSpecific));
	
	return "/Game/" + (contentFolderExists(relativePath) ? relativePath : subLevelContentRelativePath(stringutil::toStdString(groupId)));
}

//UPackage* packageFor(const TArray<FString>& groups, const FString& tile) {
//	for (auto group : groups) {
//		auto packageFilename = filenameFor(group, tile) + ".uasset";
//		if (auto package = FindPackage(nullptr, *packageFilename)) {
//			return package;
//		}
//	}
//	return nullptr;
//}

FVector getTransformed(FVector tileOriginInDecorSpace, BlockPos tilePosition, QuadrantAngle tileRotation, BlockPos tileSize) {
	auto position = conversion::blockToUe(tilePosition) - FVector(0, 0, tileOriginInDecorSpace.Z);
	auto size = conversion::blockToUe(tileSize);

	if (tileRotation == QuadrantAngle::D0) {
		position.X -= tileOriginInDecorSpace.X;
		position.Y -= tileOriginInDecorSpace.Y;
	}
	else if (tileRotation == QuadrantAngle::D90) {
		position.X -= tileOriginInDecorSpace.Y;
		position.Y += tileOriginInDecorSpace.X + size.Y;
	}
	else if (tileRotation == QuadrantAngle::D180) {
		position.X += tileOriginInDecorSpace.X + size.X;
		position.Y += tileOriginInDecorSpace.Y + size.Y;
	}
	else if (tileRotation == QuadrantAngle::D270) {
		position.X += tileOriginInDecorSpace.Y + size.X;
		position.Y -= tileOriginInDecorSpace.X;
	}
	return position;
}

const UArrowComponent* getActorOrigin(AActor& root) {
	auto components = root.GetComponentsByClass(UArrowComponent::StaticClass());
	for (const auto* a : components) {
		auto arrow = static_cast<const UArrowComponent*>(a);
		if (ADecorationAnchor::isDecorationAnchorArrowHACK(arrow)) {
			return arrow;
		}
	}
	return nullptr;
}

TOptional<FTransform> getTileOriginInDecorSpace(AActor& root) {
	if (auto arrow = getActorOrigin(root)) {
		return arrow->GetRelativeTransform();
	}
	return{};
}

FTransform placementToTransform(const Placement& placement, BlockPos tileSize) {
	const auto rotation = placement.rotation;
	const auto transformed = getTransformed(FVector::ZeroVector, placement.position, placement.rotation, tileSize);
	return{ FQuat(FVector::UpVector, -quadrantToRadians(rotation)), transformed, FVector::OneVector };
}

AActor* spawnDecorActorDeferred(UWorld& world, const TSubclassOf<AActor>& cls, AActor& parentLevel) {
	const bool isDedicatedServer = world.GetNetMode() == NM_DedicatedServer;

	if (cls->IsChildOf<ALootActor>() && parentLevel.Role == ROLE_Authority) {
		return world.SpawnActorDeferred<AActor>(cls, FTransform::Identity, &parentLevel, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	}
	if (cls->IsChildOf<ALobbyChest>() && !isDedicatedServer) {
		return world.SpawnActorDeferred<AActor>(cls, FTransform::Identity, &parentLevel, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	}
	if (!cls->IsChildOf<ALootActor>() && !cls->IsChildOf<ALobbyChest>()) {
		auto actor = world.SpawnActorDeferred<AActor>(cls, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		assert(actor->GetIsReplicated() == false); // these actors should never be replicated, they only exist server side, else handle the StaticClass type separately
		return actor;
	}
	return nullptr;
}

AActor* placeDecorActor(UWorld& world, const TilePlacement& tilePlacement, const TSubclassOf<AActor>& cls) {
	AActor* theOwner = actorquery::getFirstActor<ALovikaLevelActor>(&world);
	AActor* actor = spawnDecorActorDeferred(world, cls, *theOwner);
	if (!actor) {
		return nullptr;
	}
	UGameplayStatics::FinishSpawningActor(actor, FTransform::Identity);

	const auto origin = getTileOriginInDecorSpace(*actor);
	if (!origin) {
		FString idName(tilePlacement.tileIdHACK().c_str());
		UE_LOG(LogTemp, Error, TEXT("Can't find origin in tile: %s"), *idName);
		return nullptr;
	}

	const auto tileRotation = tilePlacement.placement().rotation;
	const auto translation = getTransformed(origin->GetLocation(), tilePlacement.placement().position, tileRotation, tilePlacement.bounds().size());
	FTransform transform{
		FQuat::MakeFromEuler(origin->GetRotation().Euler() - FQuat(FVector::UpVector, quadrantToRadians(tileRotation)).Euler()),
		translation,
		origin->GetScale3D()
	};

	if (auto rootComponent = actor->GetRootComponent()) {
		auto oldMobility = rootComponent->Mobility;
		rootComponent->SetMobility(EComponentMobility::Movable);
		actor->SetActorTransform(transform);
		rootComponent->SetMobility(oldMobility);
		TArray<USceneComponent*> sceneComponents;
		rootComponent->GetChildrenComponents(false, sceneComponents);
		for (auto component : sceneComponents) {
			component->SetMobility(oldMobility);
		}
	}
	else {
		actor->SetActorTransform(transform);
	}
	return actor;
}


AActor* placeTileDecor(UWorld& world, const TilePlacement& tilePlacement, const FString& group) {
	FString idName(tilePlacement.tileIdHACK().c_str());

	FString pathName = filenameFor(group, idName);
	auto cls = ConstructorHelpersInternal::FindOrLoadClass(pathName, AActor::StaticClass());
	if (!cls) {
		//UE_LOG(LogTemp, Warning, TEXT("Can't find asset class: %s"), *idName);
		return nullptr;
	}
	return placeDecorActor(world, tilePlacement, cls);
}

TArray<TilePlacement> getAllOriginalPlacements(const PlacementVector& placedTiles) {
	TArray<TilePlacement> placements;
	for (const auto& newPlaced : placedTiles) {
		const auto placed = newPlaced.originalPlacement().GetValue();
		placements.Add(placed);
		for (const auto& child : placed.children()) {
			placements.Add(child);
		}
	}
	return placements;
}

TArray<AActor*> placeTileDecors(UWorld& world, const game::LevelDef& levelDef) {
	FString group(levelDef.levelDef.id.c_str());
	TArray<AActor*> decors;

	for (auto&& placed : getAllOriginalPlacements(levelDef.placedTiles.placements())) {
		const auto startTime = FPlatformTime::Seconds();

		if (auto actor = placeTileDecor(world, placed, group)) {
			UE_LOG(LogTemp, Warning, TEXT("Load time for '%s': %.3f s"), UTF8_TO_TCHAR(placed.tileIdHACK().c_str()), FPlatformTime::Seconds() - startTime);
			//UE_LOG(LogTemp, Error, TEXT("Bounds: %s"), UTF8_TO_TCHAR(placed.bounds().toString().c_str()));
			decors.Add(actor);
		}
	}
	return decors;
}

void placeDecorActors(UWorld& world, const game::LevelDef& levelDef) {
	FString group(levelDef.levelDef.id.c_str());

	for (auto&& placed : getAllOriginalPlacements(levelDef.placedTiles.placements())) {
		const FString idName(placed.tileIdHACK().c_str());

		for (int count = 0; /*loop forever*/; ++count) {
			auto pathName = filenameForActor(group, idName) + "/" + idName + FString::FromInt(count);
			auto cls = ConstructorHelpersInternal::FindOrLoadClass(pathName, AActor::StaticClass());
			if (!cls) {
				break;
			}
			placeDecorActor(world, placed, cls);
		}
	}
}


AActor* spawnActorFromClass(UWorld* world, UClass* cls) {
	auto actor = world->SpawnActorDeferred<AActor>(cls, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	UGameplayStatics::FinishSpawningActor(actor, FTransform::Identity);
	if (actor) {
		auto origin = getTileOriginInDecorSpace(*actor).Get(FTransform::Identity);
		origin.SetLocation(origin.GetLocation() * -1); // @flipped the coordinates on Gavelli's request
		actor->SetActorTransform(origin);
	}
	return actor;
}


AActor* spawnActorFromPath(UWorld& world, FString path) {
	auto cls = ConstructorHelpersInternal::FindOrLoadClass(path, AActor::StaticClass());	
	return spawnActorFromClass(&world,cls);
}


void placeDecorSubLevels(UWorld& world, const game::LevelDef& levelDef) {
	game::sublevel::TileLoader loader(world, { 0, 0 });
	createDecorSubLevels(levelDef, loader);
	loader.update();
}

bool subLevelFolderExists(const std::string& folderName) {
	return contentFolderExists(subLevelContentRelativePath(folderName));
}

void addSubLevels(const std::string& folderName, const TArray<TilePlacement>& placements, game::sublevel::TileLoader& loader) {
	const auto contentRelativePath = subLevelContentRelativePath(folderName);

	if (!contentFolderExists(contentRelativePath)) {
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find SubLevels folder: %s"), *contentRelativePath);
		return;
	}
	const FString path = "/Game/" + contentRelativePath;
	for (const auto& placed : placements) {
		loader.schedule(path, placed);
	}
}

void createDecorSubLevels(const game::LevelDef& levelDef, game::sublevel::TileLoader& loader) {
	const auto missionId = Util::toLower(levelDef.levelDef.id);
	std::set<std::string> folderNames{ missionId };

	const auto& addPotentialCandidate = [&folderNames](std::string id) -> bool {
		id = Util::toLower(id);
		if (folderNames.count(id) == 0 && subLevelFolderExists(id)) {
			folderNames.insert(id);
			return true;
		}
		return false;
	};
	for (auto& def : levelDef.levelDef.data.objectGroups) {
		if (!addPotentialCandidate(def.name.lowerId + '_' + missionId)) { // E.g. genericcaves_redstonemines
			addPotentialCandidate(def.name.lowerId); // E.g. genericcaves
		}
	}
	const auto tiles = getAllOriginalPlacements(levelDef.placedTiles.placements());
	for (const auto& folderName : folderNames) {
		addSubLevels(folderName, tiles, loader);
	}
}




}
