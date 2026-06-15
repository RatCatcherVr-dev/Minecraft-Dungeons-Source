#pragma once
#include <Optional.h>
#include <Math/Vector.h>

class ABaseCharacter;
class AActor;

namespace game {
namespace mobspawn {
//Uses nav system to find a random point in radius, falling back to actor location if none was found.
FVector RandomLocationAround(const AActor*, float radius);
}
}