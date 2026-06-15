#include "Dungeons.h"
#include "GongOfWeakeningInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "AbilitySystemComponent.h"
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

AGongOfWeakeningInstance::AGongOfWeakeningInstance() {
	Effect = UWeakenGameplayEffect::StaticClass();
	TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.GongOfWeakening"));
	PowerEffects = { UDurationIncrease::StaticClass() };
}

int AGongOfWeakeningInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void AGongOfWeakeningInstance::Activate(const FPredictionKey& predictionKey) {
	const auto owner = GetOwner();

	const auto itemPowerRangeMultiplier = game::item::power::HiddenRangeIncreaseMultiplier(ItemPower);
	const auto calculatedRange = WeakenRange * itemPowerRangeMultiplier;

	if (CVarDebugDrawItems.GetValueOnGameThread()) {
		DrawDebugSphere(GetWorld(), owner->GetActorLocation(), calculatedRange, 50, FColor::Green, false, 5.0f);
	}

	const auto ownerAsCharacter = Cast<ABaseCharacter>(GetOwner());
	
	ExecuteWeakeningGameplayCue(ownerAsCharacter, calculatedRange);
	ApplyEffectToMobs(ownerAsCharacter, calculatedRange, predictionKey);

	Super::Activate(predictionKey);
}

void AGongOfWeakeningInstance::ExecuteWeakeningGameplayCue(const ABaseCharacter* ownerCharacter, const float calculatedRange) const {
	const auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();

	const auto scale = (calculatedRange / WeakenRange);

	FGameplayCueParameters parameters;
	parameters.NormalizedMagnitude = scale;
	abilitySystem->ExecuteGameplayCue(TriggerTag, parameters);
}

void AGongOfWeakeningInstance::ApplyEffectToMobs(const ABaseCharacter* ownerCharacter, const float calculatedRange, const FPredictionKey& predictionKey) const {
	auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();

	const auto spec = CreateWeakenEffectSpec(abilitySystem);
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	
	for (auto mob : GetMobsToTarget(ownerCharacter, static_cast<int32>(calculatedRange))) {
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent(), predictionKey);
	}
}

FGameplayEffectSpec AGongOfWeakeningInstance::CreateWeakenEffectSpec(UAbilitySystemComponent* abilitySystem) const {
	//const auto damageTakenMultiplier = EnemyTakeDamageMultiplier * game::item::power::HiddenDamageIncreaseMultiplier(ItemPower);
	//const auto damageDealMultiplier = EnemyDealDamageMultiplier * game::item::power::HiddenDamageReductionMultiplier(ItemPower);	
	const auto duration = game::item::type::GongOfWeakening.getDurationSeconds() * GetPowerEffect()->GetMultiplier(ItemPower);
	
	auto spec = FGameplayEffectSpec(Cast<UWeakenGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(UWeakenGameplayEffect::DurationName, duration );
	spec.SetSetByCallerMagnitude(UWeakenGameplayEffect::DamageTakenName, EnemyTakeDamageMultiplier);
	spec.SetSetByCallerMagnitude(UWeakenGameplayEffect::DamageDealtName, 1.f/ EnemyDealDamageMultiplier);
	return spec;
}

TArray<AMobCharacter*> AGongOfWeakeningInstance::GetMobsToTarget(const ABaseCharacter* ownerCharacter, const int32 range) const {
	const auto mobsInRange = actorquery::getNearbyActors<AMobCharacter>(ownerCharacter, range);
	return mobsInRange.FilterByPredicate([=](const AMobCharacter* mob) { return characterquery::is::targetable(mob) && !ownerCharacter->IsFriendlyTowards(mob) && mob->IsAlive(); });
}