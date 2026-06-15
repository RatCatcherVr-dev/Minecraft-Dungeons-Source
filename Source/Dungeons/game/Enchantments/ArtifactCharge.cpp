#include "Dungeons.h"
#include "ArtifactCharge.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/item/instance/AItemInstance.h"

UArtifactCharge::UArtifactCharge() {
	TypeId = EEnchantmentTypeID::ArtifactCharge;

	
	LevelMultiplier = [this](int level) -> float {
		return level;
	};
	
	MultiplierFormatter = valueformat::asConstant;
}

UArtifactChargeGameplayEffect::UArtifactChargeGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Ranged.AutoCharge"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Enchantment.ArtifactCharge"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ArtifactCharge"), 0, 1);
}

void UArtifactCharge::OnItemActivated(const AItemInstance * instance, FPredictionKey key) {
	if (!instance->GetItemType().isPermanent()) {
		return;
	}
	if (EffectPerLevel.Num() <= 0) {
		return;
	}

	auto abilitySystem = GetAbilitySystemComponent();
	const auto index = FMath::Min(Level, EffectPerLevel.Num()) - 1;
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, EffectPerLevel[index], Level);
	spec.StackCount = LevelMultiplier(Level);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, key);
}

void UArtifactCharge::OnRangedAttackEnded(bool, bool didSpawnProjectiles) {
	if (GetOwnerRole() == ROLE_Authority && didSpawnProjectiles) {
		auto abilitySystem = GetAbilitySystemComponent();
		for (auto effect : abilitySystem->GetActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Ranged.AutoCharge")))))) {
			if (!abilitySystem->GetActiveGameplayEffect(effect)->Spec.Def->IsA<UArtifactChargeGameplayEffect>()) return;
		}

		GetAbilitySystemComponent()->RemoveActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.Enchantment.ArtifactCharge"))), 1);
	}
}

void UArtifactCharge::OnEnd() {
	GetAbilitySystemComponent()->RemoveActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.Enchantment.ArtifactCharge"))));
}