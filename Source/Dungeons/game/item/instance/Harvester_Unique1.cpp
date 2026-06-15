#include "Dungeons.h"
#include "game/Enchantments/Poisoned.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "Harvester_Unique1.h"


AHarvester_Unique1::AHarvester_Unique1(){
	PoisonCloudClass = APoisonCloud::StaticClass();
	PoisonCloudDamageEffect = UPoisonItemCloudDamageGameplayEffect::StaticClass();
	ExplosionTriggerTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Harvester.Unique1");
}

void AHarvester_Unique1::OnPostMobDeath(AMobCharacter* mob) {
	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	UPoisonCloudSpawnHelper::SpawnPoisonCloud(PoisonCloudClass, PoisonCloudDamageEffect, GetOwner(), mob->GetActorLocation(), ItemPower, DamagePerSecond * ItemPowerMultiplier, 2.5f);
}
