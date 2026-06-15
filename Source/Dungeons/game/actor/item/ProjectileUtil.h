#pragma once
#include "Runtime/Core/Public/Math/Color.h"
#include <Kismet/BlueprintFunctionLibrary.h>

class ABaseProjectile;
class UParticleSystemComponent;
struct FLinearColor;

namespace game { namespace item { namespace util { 

FLinearColor GetProjectileTrailColor(ABaseProjectile*);
void SetProjectileTrailColor(ABaseProjectile*, const FLinearColor&);

}}}