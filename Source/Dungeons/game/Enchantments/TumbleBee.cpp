// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "TumbleBee.h"
#include "game/actor/character/mob/MobSummonHelper.h"
#include "world/entity/EntityTypes.h"

UTumbleBee::UTumbleBee() {
	TypeId = EEnchantmentTypeID::TumbleBee;
	SummonHelper->MobToSpawn = EntityType::Bee;
	SummonHelper->SpawnCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon.Bee");
	SpawnDelaySeconds = 0.1f;
	BaseTriggerChance = 0.33f;
	TriggerChanceIncreasePerLevel = 0.34f;
	MaxNumMobs = 3;
}

void UTumbleBee::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {
	if (GetOwnerRole() == ROLE_Authority) {
		FVector locationFallback = GetOwner()->GetActorLocation();
		TWeakObjectPtr<AActor> weakOwner(GetOwner());
		game::mobspawn::TransformProvider location = [weakOwner, locationFallback]() { return FTransform(weakOwner.IsValid() ? weakOwner->GetActorLocation() : locationFallback); };
		static FRandomStream rnd(FMath::Rand());
		AttemptMobSummon(location, rnd);
	}
}