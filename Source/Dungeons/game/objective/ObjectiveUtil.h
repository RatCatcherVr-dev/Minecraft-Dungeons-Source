#pragma once

#include "CommonTypes.h"
#include "ObjectiveLocations.h"
#include "RegionFinder.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "world/phys/Vec3.h"
#include "world/Facing.h"

class AActor;
class UClass;
class UWorld;
class USoundCue;

namespace io { struct ObjectiveInfo; }

namespace game { namespace objective {

FString identifiableName(const io::ObjectiveInfo&);
FString identifiableDescription(const io::ObjectiveInfo&);

Facing::Name outwardFace(Vec3 point, const BlockCuboid& tileBounds);

AActor* placeObject(UWorld&, FString path, Vec3, float degrees);
AActor* placeObject(UWorld&, UClass&, Vec3, float degrees);

USoundCue* loadSound(FString path);

FObjectiveLocations toObjectiveLocations(const RegionFinder&, const TOptional<io::RegionLocator>&);

}}
