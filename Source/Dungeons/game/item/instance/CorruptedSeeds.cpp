#include "Dungeons.h"
#include "CorruptedSeeds.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/item/ItemBulletPoint.h"
#include "util/CharacterQuery.h"
#include "world/entity/MobTags.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UCorruptedSeedsEntangleEffect::UCorruptedSeedsEntangleEffect(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	bRequireModifierSuccessToTriggerCues = true;

	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = TEXT("Duration");
	DurationMagnitude = durationMagnitude;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	FSetByCallerFloat speedMagnitude;
	speedInfo.ModifierMagnitude = FScalableFloat(0.0f);
	speedInfo.ModifierOp = EGameplayModOp::Override;

	Modifiers.Add(speedInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Entangled"), 0, 1);
}

UCorruptedSeedsPoisonEffect::UCorruptedSeedsPoisonEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = TEXT("Duration");
	DurationMagnitude = durationMagnitude;

	bExecutePeriodicEffectOnApplication = true;
	bSuppressStackingCues = true;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Poison")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Poisoned"), 0, 1);
}

ACorruptedSeeds::ACorruptedSeeds() {
	EntangleEffect = UCorruptedSeedsEntangleEffect::StaticClass();
	PoisonEffect = UCorruptedSeedsPoisonEffect::StaticClass();
	TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.CorruptedSeed"));
	PowerEffects = { UItemDamagePerSecondIncrease::StaticClass() };
}

void ACorruptedSeeds::PopulateBulletPoints(TArray<FItemBulletPoint>& intoList, const ABaseCharacter& owner) const {
	Super::PopulateBulletPoints(intoList, owner);
	intoList.Add(FItemBulletPoint::CreateTargetsMaxiumumBulletPoint(TargetCount));
}

void ACorruptedSeeds::Activate(const FPredictionKey& predictionKey) {
	auto abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	const float powerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);

	FGameplayEffectSpec entangleSpec( Cast<UCorruptedSeedsEntangleEffect>(EntangleEffect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	entangleSpec.SetSetByCallerMagnitude(FName("Duration"), game::item::type::CorruptedSeeds.getDurationSeconds());

	FGameplayEffectSpec poisonSpec( Cast<UCorruptedSeedsPoisonEffect>(PoisonEffect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	poisonSpec.SetSetByCallerMagnitude(FName("Duration"), game::item::type::CorruptedSeeds.getDurationSeconds());
	poisonSpec.SetSetByCallerMagnitude(effects::HealthName, -PoisonDamageScale * powerMultiplier);
	poisonSpec.Period = PoisonDamagePeriod;

	auto targets = GetMobsInRange();
	for (auto mob : targets) {
		auto mobAbility = mob->GetAbilitySystemComponent();
		mobAbility->ApplyGameplayEffectSpecToSelf(entangleSpec, predictionKey);
		mobAbility->ApplyGameplayEffectSpecToSelf(poisonSpec, predictionKey);
	}

	FGameplayCueParameters parameters;
	parameters.Instigator = GetOwner();
	parameters.SourceObject = GetOwner();
	parameters.TargetAttachComponent = GetOwner()->GetRootComponent();
	abilitySystem->ExecuteGameplayCue(TriggerTag, parameters);

	Super::Activate(predictionKey);
}

float ACorruptedSeeds::GetStats(EItemStats stat) const {	
	switch (stat) {
		case EItemStats::DamagePerSecond:
			return PoisonDamageScale / PoisonDamagePeriod;
		default:
			break;
	}
	return -1;
}

TArray<AMobCharacter*> ACorruptedSeeds::GetMobsInRange()
{
	TArray<AMobCharacter*> Targets;

	auto owner = Cast<ABaseCharacter>(GetOwner());

	auto mobsInRange = actorquery::getNearbyActors<AMobCharacter>(owner, Radius);
	mobsInRange.Sort([owner](const AMobCharacter& a, const AMobCharacter& b) { 
		return owner->GetSquaredDistanceTo(&a) < owner->GetSquaredDistanceTo(&b); 
	});

	mobsInRange = mobsInRange.FilterByPredicate([=](const AMobCharacter* mob) {
		return characterquery::is::targetable(mob) && owner->IsHostileTowards(mob) && mob->IsAlive()
			&& !mob->HasTag(MobTags::HashTag_Miniboss)
			&& !mob->HasTag(MobTags::HashTag_Immobile)
			&& !mob->HasTag(MobTags::HashTag_Floating);
	});

	for (int i = 0; i < FMath::Min(TargetCount, mobsInRange.Num()); ++i) {
		Targets.Add(mobsInRange[i]);
	}

	return Targets;
}
