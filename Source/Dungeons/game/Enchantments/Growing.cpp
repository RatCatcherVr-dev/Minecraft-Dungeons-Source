// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Growing.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "FuseShot.h"
#include "game/actor/item/ProjectileUtil.h"
#include "game/component/GrowingArrowComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "../component/PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"

UGrowing::UGrowing() {
	TypeId = EEnchantmentTypeID::Growing;
	LevelMultiplier = [](int level) -> float {
		return 0.25f * (float)level;
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UGrowing::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (auto characterOwner = GetCharacterOwner()) 
	{
		UGrowingArrowComponent* pGrowComponent = projectile->FindComponentByClass<UGrowingArrowComponent>();

		if (!pGrowComponent)
		{
			pGrowComponent = NewObject<UGrowingArrowComponent>(projectile,TEXT("Growing arrow"));
			pGrowComponent->RegisterComponent();
		}

		pGrowComponent->SetMaxDistance(IsOwnerMob() ? MobMaxDistance : MaxDistance);
		pGrowComponent->Activate(true);
		pGrowComponent->SetMultiplier(IsOwnerMob() ? MobDamageFractionBonus : LevelMultiplier(Level));

		UGameplayStatics::SpawnSoundAttached(GrowingSound, characterOwner->GetRootComponent());
	}
}

void UGrowing::OnResetRangedProjectile(ABaseProjectile* fromProjectile)
{
	if (auto characterOwner = GetCharacterOwner())
	{
		auto pGrowComponent = fromProjectile->FindComponentByClass<UGrowingArrowComponent>();
		if (pGrowComponent)
		{
			pGrowComponent->Deactivate();
		}
	}
}


