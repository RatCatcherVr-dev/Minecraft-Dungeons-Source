#include "Dungeons.h"
#include "ProjectileUtil.h"
#include "BaseProjectile.h"
#include "Runtime/Core/Public/Math/Color.h"
#include "game/util/ComponentUtils.h"

namespace game { namespace item { namespace util {

FLinearColor GetProjectileTrailColor(ABaseProjectile* projectile) {
	if (auto PS = componentutils::GetComponentOfName<UParticleSystemComponent>("P_Combat_Arrow", projectile)) {
		FLinearColor color;
		if (PS->GetColorParameter("Color", color)) {
			return color;
		}
	}

	return FLinearColor::Black;
}

void SetProjectileTrailColor(ABaseProjectile* projectile, const FLinearColor& color) {
	if (auto PS = componentutils::GetComponentOfName<UParticleSystemComponent>("P_Combat_Arrow", projectile)) {
		PS->SetColorParameter("Color", color);
	}
}

}}}