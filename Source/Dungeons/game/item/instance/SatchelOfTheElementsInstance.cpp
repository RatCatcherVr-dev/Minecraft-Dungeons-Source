#include "Dungeons.h"
#include "SatchelOfTheElementsInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/item/ItemBulletPoint.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/Enchantments/Freezing.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

ASatchelOfTheElementsInstance::ASatchelOfTheElementsInstance() {
	bHasManualCooldownActivation = true;
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	TargetCountCooldownPercentage = { 0.1f, 1.0f };
	bCanFail = true;
}

void ASatchelOfTheElementsInstance::PopulateBulletPoints(TArray<FItemBulletPoint>& intoList, const ABaseCharacter& owner) const {
	Super::PopulateBulletPoints(intoList, owner);
	intoList.Add(FItemBulletPoint::CreateTargetsMaxiumumBulletPoint(MaxMobAttackTotal));
}

float ASatchelOfTheElementsInstance::GetStats(EItemStats stat) const {
	switch (stat) {	
	case EItemStats::LowestDamage:
		return FMath::Min(LightningDamage, FireDamage);
	case EItemStats::HighestDamage:
		return FMath::Max(LightningDamage, FireDamage);
	}
	return -1;
}


ESatchelEffect ASatchelOfTheElementsInstance::GetRandomEffect(){
	const float randValue = FMath::FRand();
	
	if (randValue < FireChanceWeight) return ESatchelEffect::Burning;
	else if (randValue < FireChanceWeight + LightningChanceWeight) return ESatchelEffect::Shocking;

	return ESatchelEffect::Freezing;
}

void ASatchelOfTheElementsInstance::Activate(const FPredictionKey& predictionKey) {		
	if (Role == ROLE_Authority) {
		auto owningCharacter = Cast<APlayerCharacter>(GetPlayerOwner());
		TArray<AMobCharacter*> actors = actorquery::getNearbyInstanceTrackedActors<AMobCharacter>(
			GetPlayerOwner(),
			EffectRange,
			[=](AMobCharacter* mob) { return !owningCharacter->IsFriendlyTowards(mob) && mob->IsAlive() && mob->IsTargetable(); }
		);
		actors.Sort([&owningCharacter](const AMobCharacter& a, const AMobCharacter& b) -> bool {
			return actorquery::getActorDistance2D(owningCharacter, &a) < actorquery::getActorDistance2D(owningCharacter, &b);
		});
		const ESatchelEffect effect = GetRandomEffect();
		for (int i = 0; i < FMath::Min(actors.Num(), MaxMobAttackTotal); i++) {
			switch (effect) {
			case ESatchelEffect::Burning:
				Burn(*actors[i]);
				break;
			case ESatchelEffect::Freezing:
				Freeze(*actors[i]);
				break;
			case ESatchelEffect::Shocking:
				LightningStrike(*actors[i]);
				break;
			}
		}

		const int NumTargeted = actors.Num();
		if (NumTargeted <= 0){
			TOptional<FGameplayTag> activatedTag = [effect]() -> TOptional<FGameplayTag> {
				switch (effect) {
				case ESatchelEffect::Burning:
					return FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SatchelOfTheElements.Missed.Burning");
				case ESatchelEffect::Freezing:
					return FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SatchelOfTheElements.Missed.Freezing");
				case ESatchelEffect::Shocking:
					return FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SatchelOfTheElements.Missed.Shocking");				
				default:			
					return {};
				}
			}();

			if(activatedTag.IsSet()){
				FGameplayCueParameters params;
				params.Location = owningCharacter->GetActorLocation();
				params.Instigator = owningCharacter;
				params.RawMagnitude = EffectRange;
				owningCharacter->GetAbilitySystemComponent()->ExecuteGameplayCue(activatedTag.GetValue(), params);
			}
		}
		else 
		{
			ActivationSucceeded(predictionKey);
		}

		Cooldown().TriggerCooldown(CalculateCooldown(NumTargeted));
	}

	Super::Activate(predictionKey);
}

float ASatchelOfTheElementsInstance::CalculateCooldown(int targetCount){
	const int index = FMath::Min(targetCount, TargetCountCooldownPercentage.Num() - 1);
	return Super::CalculateCooldown() * TargetCountCooldownPercentage[index];
}


void ASatchelOfTheElementsInstance::LightningStrike(AMobCharacter& target) {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
	const APlayerCharacter* owner = GetPlayerOwner();
	UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent();
	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);

	FGameplayEffectSpec spec(Cast<USatchelOfTheElementsLightningStrikeGameplayEffect>(USatchelOfTheElementsLightningStrikeGameplayEffect::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::HealthName, -LightningDamage * ItemPowerMultiplier);
	AddPushbackToSpec(spec, target);
	abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target.GetAbilitySystemComponent());
}

void ASatchelOfTheElementsInstance::Burn(AMobCharacter& target) {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	UAbilitySystemComponent* abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();
	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	const float FireDamagePerPeriod = -FireDamage / (EffectDuration / FirePeriod) * ItemPowerMultiplier;

	FGameplayEffectSpec spec(Cast<USatchelOfTheElementsBurningGameplayEffect>(USatchelOfTheElementsBurningGameplayEffect::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::HealthName, FireDamagePerPeriod);
	spec.SetSetByCallerMagnitude(TEXT("FireAspectDuration"), EffectDuration);
	spec.Period = FirePeriod;
	AddPushbackToSpec(spec, target);
	abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target.GetAbilitySystemComponent());
}

void ASatchelOfTheElementsInstance::Freeze(AMobCharacter& target) {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	UAbilitySystemComponent* abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UFreezingGameplayEffect>(UFreezingGameplayEffect::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);

	spec.SetSetByCallerMagnitude(USlowMultiplicativeResistanceModCalculation::SetByCallerKey, 1 / FreezeSpeedPercentage);
	spec.SetSetByCallerMagnitude(effects::DurationName, FreezeEffectDuration);
	abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target.GetAbilitySystemComponent());
}

void ASatchelOfTheElementsInstance::AddPushbackToSpec(FGameplayEffectSpec& spec, const AActor& target)
{
	FPushback push;
	push.pushbackStrength = 1.0f;
	push.enablePushback = true;
	effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, *this, target, 1.0f, 1.5f));
}

USatchelOfTheElementsLightningStrikeGameplayEffect::USatchelOfTheElementsLightningStrikeGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.TargetingStrikeItem.MainStrikeTarget"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
}

USatchelOfTheElementsBurningGameplayEffect::USatchelOfTheElementsBurningGameplayEffect() : Super() {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UItemDamageModCalculation::StaticClass();

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers[0] = healthInfo;
	GameplayCues[0] = FGameplayEffectCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Burning.SatchelOfTheElements"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);

}