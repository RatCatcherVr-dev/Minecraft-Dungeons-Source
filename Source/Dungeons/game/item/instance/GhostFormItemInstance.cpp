#include "Dungeons.h"
#include "GhostFormItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/RangedAttackComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

const FName UGhostCloakGameplayEffect::GhostFormDurationKey(TEXT("GhostFormDurationEffect"));
const FName UGhostCloakGameplayEffect::GhostFormSpeedKey(TEXT("GhostFormSpeedEffect"));

AGhostFormItemInstance::AGhostFormItemInstance() {
	Effect = UGhostCloakGameplayEffect::StaticClass();
	PowerEffects = { USpeedIncrease::StaticClass() };
	SharedPassiveCooldown = 0.f;
}

void AGhostFormItemInstance::BeginPlay() {
	Super::BeginPlay();
	if (HasAuthority()) {
		if (auto meleeAttackComp = GetOwner()->FindComponentByClass<UMeleeAttackComponent>()) {
			meleeAttackComp->OnPlayerMeleeAttack.AddUObject(this, &AGhostFormItemInstance::OnPlayerAttack);
		}
		if (auto rangedAttackComp = GetOwner()->FindComponentByClass<URangedAttackComponent>()) {
			rangedAttackComp->OnPlayerRangedAttack.AddUObject(this, &AGhostFormItemInstance::OnPlayerAttack);
		}
	}
}

int AGhostFormItemInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void AGhostFormItemInstance::Activate(const FPredictionKey& predictionKey) {
	const auto owner = GetOwner();
	auto abilitySystem = Cast<APlayerCharacter>(owner)->GetAbilitySystemComponent();

	const auto itemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	const auto duration = game::item::type::GhostCloak.getDurationSeconds();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(UGhostCloakGameplayEffect::GhostFormSpeedKey, SpeedMultiplierBase * GetPowerEffect()->GetMultiplier(ItemPower));
	spec.SetSetByCallerMagnitude(effects::DurationName, duration);

	EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);
}

void AGhostFormItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGhostFormItemInstance::OnPlayerAttack(const APlayerCharacter* player) {
	auto abilitySystem = Cast<APlayerCharacter>(GetOwner())->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(EffectHandle);
}

UGhostCloakGameplayEffect::UGhostCloakGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = GhostFormSpeedKey;

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedInfo);

	//Remove parent tag and add our derived tag
	InheritableOwnedTagsContainer.RemoveTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk.GhostCloak"));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
	
	//Get rid of parent underived tags.
	GameplayCues.Empty();

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Ghost.GhostCloak"), 0, 1);
	GameplayCues.Last().GameplayCueTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.GhostCloak"));
}