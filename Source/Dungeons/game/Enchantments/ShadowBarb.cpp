#include "Dungeons.h"
#include "ShadowBarb.h"
#include "game/item/instance/BackstabbersBrewInstance.h"
#include "game/util/ActorQuery.h"
#include <GameplayEffect.h>
#include "../component/RangedAttackComponent.h"

UShadowBarbRanged::UShadowBarbRanged()
{
	TypeId = EEnchantmentTypeID::ShadowBarbRanged;
	PredictiveExecution = true;
}

void UShadowBarbRanged::OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream)
{
	if (auto rangedAttackComponent = GetOwner()->FindComponentByClass<URangedAttackComponent>()) {
		if (rangedAttackComponent->bRemoveInvisibilityAfterAttack) //skip if something already prevented removal
		{
			rangedAttackComponent->bRemoveInvisibilityAfterAttack = remainingAttacks <= 0;
			if (remainingAttacks > 0)
			{
				remainingAttacks--;
			}
		}
	}
}

void UShadowBarbRanged::OnInvisibilityGained()
{
	remainingAttacks = 1; //LevelMultiplier(Level);
}
