#include "Dungeons.h"
#include "HarvesterInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include <AbilitySystemComponent.h>
#include <IConsoleManager.h>
#include "game/component/SoulComponent.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/util/Pushback.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;


UHarvesterDamageGameplayEffect::UHarvesterDamageGameplayEffect()
{
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Soul")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Harvester")));
}



AHarvesterInstance::AHarvesterInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	ExplosionTriggerTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Harvester");
}


float AHarvesterInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return GetItemType().getSoulCost()*DamagePerSoul;
	}
	return -1;
}

void AHarvesterInstance::ExecuteHarvesterGameplayCue(const APlayerCharacter* player) {
	auto* abilitySystem = player->GetAbilitySystemComponent();
	check(abilitySystem);

	const float soulCount = GetSoulActivationCost();

	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	const float SoulMagnitude = 1.0f;
	const float Damage = DamagePerSoul * soulCount * ItemPowerMultiplier;
	const float ExplosionRange = SoulMagnitude * ExplosionDamageRange;

	FGameplayCueParameters parameters;
	parameters.NormalizedMagnitude = SoulMagnitude;
	parameters.Location = player->GetActorLocation();
	abilitySystem->ExecuteGameplayCue(ExplosionTriggerTag, parameters);
}

void AHarvesterInstance::ApplyHarvesterEffectToMobs(const APlayerCharacter* player) {
	auto* abilitySystem = player->GetAbilitySystemComponent();
	check(abilitySystem);

	const float SoulMagnitude = 1.0f;
	const float Damage = DamagePerSoul * GetSoulActivationCost() * GetPowerEffect()->GetMultiplier(ItemPower);
	const float ExplosionRange = SoulMagnitude * ExplosionDamageRange;

	if (CVarDebugDrawItems.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), player->GetActorLocation(), ExplosionRange, 50, FColor::Green, false, 5.0f);

	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UHarvesterDamageGameplayEffect>(abilitySystem, ItemPower);
	FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
	spec.SetSetByCallerMagnitude(effects::HealthName, -Damage);
	context->AddInstigator(GetOwner(), this);
	context->AddOrigin(GetOwner()->GetActorLocation());

	for (auto actor : actorquery::getNearbyActors<AMobCharacter>(player, ExplosionRange)) {
		effects::StorePushbackInNormal(spec, pushback::getLaunchVector(OnKillPushback, *GetOwner(), *actor, SoulMagnitude, 0.0f));
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, actor->GetAbilitySystemComponent());
	}
}

void AHarvesterInstance::Activate(const FPredictionKey& predictionKey) {
	auto owner = GetPlayerOwner();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);

	ExecuteHarvesterGameplayCue(owner);
	if (HasAuthority()) {
		ApplyHarvesterEffectToMobs(owner);
	}

	Super::Activate(predictionKey);
}

void AHarvesterInstance::OnPostMobDeath(AMobCharacter* mob) {
}
