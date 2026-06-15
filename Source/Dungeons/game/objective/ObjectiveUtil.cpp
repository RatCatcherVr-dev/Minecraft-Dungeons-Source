#include "Dungeons.h"
#include "ObjectiveUtil.h"
#include "Objective.h"
#include "game/Conversion.h"
#include "lovika/BlockCuboid.h"
#include "world/Facing.h"
#include <ConstructorHelpers.h>
#include <Sound/SoundCue.h>
#include "Assets/DungeonsAssetManager.h"

namespace game { namespace objective {

FString translationOrFallback(const FString& translation, const FString& fallback) {
	static const FString MissingTranslationText = "<MISSING STRING TABLE ENTRY>";
	return translation != MissingTranslationText ? translation : fallback;
}

FString identifiableName(const io::ObjectiveInfo& info) {
	return translationOrFallback(info.name.ToString(), "**" + info.id.Get(info.rawName));
}

FString identifiableDescription(const io::ObjectiveInfo& info) {
	return translationOrFallback(info.description.ToString(), "**" + info.rawDescription);
}

Facing::Name outwardFace(Vec3 point, const BlockCuboid& tileBounds) {
	struct Dist {
		Facing::Name facing;
		float dist;
		bool operator<(const Dist& rhs) const { return dist < rhs.dist; }
	};
	const std::array<Dist, 4> distances {{
		{ Facing::Name::NORTH, std::abs(point.z - tileBounds.minInclusive.z) },
		{ Facing::Name::SOUTH, std::abs(point.z - tileBounds.maxExclusive.z) },
		{ Facing::Name::WEST,  std::abs(point.x - tileBounds.minInclusive.x) },
		{ Facing::Name::EAST,  std::abs(point.x - tileBounds.maxExclusive.x) }
	}};
	return std::min_element(distances.begin(), distances.end())->facing;
}

AActor* placeObject(UWorld& world, FString path, Vec3 pos, float degrees) {
	if (auto cls = ConstructorHelpersInternal::FindOrLoadClass(path, AActor::StaticClass())) {
		return placeObject(world, *cls, pos, degrees);
	}
	return nullptr;
}

AActor* placeObject(UWorld& world, UClass& cls, Vec3 pos, float degrees) {
	auto rotator = FRotator::MakeFromEuler(FVector(0, 0, degrees));
	if (auto object = world.SpawnActor<AActor>(&cls, conversion::posToUe(pos), rotator)) {
		object->SetReplicates(true);
		return object;
	}
	return nullptr;
}

USoundCue* loadSound(FString path) 
{
	FSoftObjectPath objectPath = FSoftObjectPath(path);
	StaticLoadObject(UParticleSystem::StaticClass(), NULL, *objectPath.GetAssetPathString());
	USoundCue* SoundCue = Cast<USoundCue>(objectPath.ResolveObject());
	return SoundCue;//ConstructorHelpersInternal::FindOrLoadObject<USoundCue>(SoundCue);
}

FObjectiveLocations toObjectiveLocations(const RegionFinder& regionFinder, const TOptional<io::RegionLocator>& maybeLocator) {
	if (maybeLocator) {
		const auto regions = regionFinder.get(maybeLocator.GetValue());
		if (!regions.empty()) {
			return algo::map_tarray(regions, RETLAMBDA(conversion::posToUe(centerFloor(it.area()))));
		}
	}
	return {};
}

}}
