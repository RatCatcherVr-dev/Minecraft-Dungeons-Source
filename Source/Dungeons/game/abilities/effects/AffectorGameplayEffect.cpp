#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "AffectorGameplayEffect.h"
#include "game/abilities/attributes/AffectorAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ArmorProperties/ItemCooldownDecrease.h"

namespace affector { namespace effect {

const EffectType& damageEffectType() {
	static const EffectType et("DAMAGE_AFFECTOR", UAffectorDamageEffect::StaticClass());
	return et;
}

const EffectType& maxHealthEffectType() {
	static const EffectType et("MAX_HEALTH_AFFECTOR", UAffectorHealthEffect::StaticClass());
	return et;
}

const EffectType& speedEffectType() {
	static const EffectType et("SPEED_AFFECTOR", UAffectorSpeedEffect::StaticClass());
	return et;
}

const EffectType& artifactCooldownType() {
	static const EffectType et(itemcooldowneffect::DataName, UItemCooldownDecreaseGameplayEffect::StaticClass(), [](float magnitude) { return 1.0f / magnitude; });
	return et;
}

const EffectType& underwaterEffectType() {
	static const EffectType et("UNDERWATER_AFFECTOR", UAffectorUnderwaterEffect::StaticClass());
	return et;
}

EffectType::EffectType(const FName& dataName, const TSubclassOf<UGameplayEffect>& effectClass, const EffectMagnitudeModifier& magnitudeModifier):
	DataName(dataName),
	EffectClass(effectClass),
	MagnitudeModifier(magnitudeModifier) {
}

void applyKeyValEffect(UAbilitySystemComponent* as, const AffectorInstance& ai) {
	if (!as->GetOwner()->HasAuthority()) {
		UE_LOG(LogDungeons, Error, TEXT("Applying affector effect on client '%s' is not allowed - skipping."), *ai.Type.RuleId);
		return;
	}
	const auto* et = ai.Type.EffectType();
	if (et == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Applying effect of a affector type without effect: %s"), *ai.Type.RuleId);
		return;
	}
	const auto value = ai.Data.AsFloat();
	auto spec = FGameplayEffectSpec(et->EffectClass.GetDefaultObject(), as->MakeEffectContext());
	spec.SetSetByCallerMagnitude(et->DataName, et->MagnitudeModifier(value));
	as->ApplyGameplayEffectSpecToSelf(spec);
}

void applyEffects(UAbilitySystemComponent* as, bool bApplyOnlyDefaults) {
	const auto& affectors = get(as->GetWorld());
	for (const AffectorInstance& ai : affectors.GetActiveWithEffect(as->GetOwner()->IsA<APlayerCharacter>() ? Player : Mob)) {
		if (!bApplyOnlyDefaults || ai.Data.IsDefault())
		{
			applyKeyValEffect(as, ai);
		}
	}
}

void applyEffect(UAbilitySystemComponent* as, const AffectorType& type) {
	const auto& affectors = get(as->GetWorld());
	if (const auto* ai = affectors.Find(type)) {
		FGameplayEffectQuery q;
		q.EffectDefinition = ai->Type.EffectType()->EffectClass;
		as->RemoveActiveEffects(q);
		applyKeyValEffect(as, *ai);
	}
}

void initEffect(UGameplayEffect& effect) {
	effect.DurationPolicy = EGameplayEffectDurationType::Infinite;
	effect.StackLimitCount = 1;
	effect.StackingType = EGameplayEffectStackingType::AggregateByTarget;
	effect.StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	effect.StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	effect.bExecutePeriodicEffectOnApplication = false;
	effect.bRequireModifierSuccessToTriggerCues = true;
	effect.InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
}

}} // affector // effect


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Effects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UAffectorHealthEffect::UAffectorHealthEffect() {
	affector::effect::initEffect(*this);
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat baseHealthModifier;
	baseHealthModifier.CalculationClassMagnitude = UAffectorMaxHealthModCalculation::StaticClass();
	
	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	info.ModifierMagnitude = baseHealthModifier;
	info.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(info);
}

UAffectorDamageEffect::UAffectorDamageEffect() {
	affector::effect::initEffect(*this);

	FSetByCallerFloat setByCaller;
	setByCaller.DataName = affector::effect::damageEffectType().DataName;

	FGameplayModifierInfo info;
	info.Attribute = UAffectorAttributeSet::DealDamageMulAttribute();
	info.ModifierMagnitude = setByCaller;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	Modifiers.Add(info);
}

UAffectorSpeedEffect::UAffectorSpeedEffect() {
	affector::effect::initEffect(*this);

	FSetByCallerFloat setByCaller;
	setByCaller.DataName = affector::effect::speedEffectType().DataName;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	info.ModifierMagnitude = setByCaller;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	Modifiers.Add(info);
}

UAffectorUnderwaterEffect::UAffectorUnderwaterEffect() {
	affector::effect::initEffect(*this);
	
	bRequireModifierSuccessToTriggerCues = true;

	FSetByCallerFloat setByCaller;
	setByCaller.DataName = affector::effect::underwaterEffectType().DataName;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	info.ModifierMagnitude = setByCaller;
	info.ModifierOp = EGameplayModOp::Type::Division;
	Modifiers.Add(info);

	FGameplayModifierInfo infoGravity;
	infoGravity.Attribute = UMovementAttributeSet::GravityAttribute();
	FScalableFloat ScalableFloatGravity;
	ScalableFloatGravity.SetValue(0.12f);
	FGameplayEffectModifierMagnitude ModifierGravityMagnitude(ScalableFloatGravity);
	infoGravity.ModifierMagnitude = ModifierGravityMagnitude;
	infoGravity.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	Modifiers.Add(infoGravity);

	FGameplayModifierInfo infoDodge;
	infoDodge.Attribute = UMovementAttributeSet::DodgeSpeedAttribute();
	FScalableFloat ScalableFloatDodge;
	ScalableFloatDodge.SetValue(0.2f);
	FGameplayEffectModifierMagnitude ModifierDodgeMagnitude(ScalableFloatDodge);
	infoDodge.ModifierMagnitude = ModifierDodgeMagnitude;
	infoDodge.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(infoDodge);

	FGameplayModifierInfo infoPushback;
	infoPushback.Attribute = UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute();
	FScalableFloat ScalableFloatPushback;
	ScalableFloatPushback.SetValue(3.5f);
	FGameplayEffectModifierMagnitude ModifierPushbackMagnitude(ScalableFloatPushback);
	infoPushback.ModifierMagnitude = ModifierPushbackMagnitude;
	infoPushback.ModifierOp = EGameplayModOp::Type::Division;
	Modifiers.Add(infoPushback);

	FGameplayModifierInfo infoWindResist;
	infoWindResist.Attribute = UResistanceAttributeSet::WindResistanceMagnitudeAttribute();
	FScalableFloat ScalableFloatWindResist;
	ScalableFloatWindResist.SetValue(3.5f);
	FGameplayEffectModifierMagnitude ModifierWindResistMagnitude(ScalableFloatWindResist);
	infoWindResist.ModifierMagnitude = ModifierWindResistMagnitude;
	infoWindResist.ModifierOp = EGameplayModOp::Type::Division;
	Modifiers.Add(infoWindResist);

	FGameplayModifierInfo infoPushbackZ;
	infoPushbackZ.Attribute = UResistanceAttributeSet::PushbackZClampMagnitudeAttribute();
	FScalableFloat ScalableFloatPushbackZClamp;
	ScalableFloatPushbackZClamp.SetValue(200.0f);
	FGameplayEffectModifierMagnitude ModifierPushbackZClampMagnitude(ScalableFloatPushbackZClamp);
	infoPushbackZ.ModifierMagnitude = ModifierPushbackZClampMagnitude;
	infoPushbackZ.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(infoPushbackZ);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Underwater")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.DragOnArrows")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Resistance.PushVolume.Wind")));
	OngoingTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Immunity.Underwater")));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Modifiers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UAffectorMaxHealthModCalculation::UAffectorMaxHealthModCalculation() 	
	: MaxHealthAttribute(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true) {
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(MaxHealthAttribute);
}

float UAffectorMaxHealthModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& spec) const {
	auto maxHealth = 1.f;
	if (const auto* captureSpec = spec.CapturedRelevantAttributes.FindCaptureSpecByDefinition(MaxHealthAttribute, true)) {
		if (!captureSpec->AttemptCalculateAttributeBaseValue(maxHealth)) {
			UE_LOG(LogTemp, Warning, TEXT("Could not find the max health base value for affector modification"));
		}
	}
	const auto mul = spec.GetSetByCallerMagnitude(affector::effect::maxHealthEffectType().DataName, true, 1);
	return maxHealth * mul;
}
