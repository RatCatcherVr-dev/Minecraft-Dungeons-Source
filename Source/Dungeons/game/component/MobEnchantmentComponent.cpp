#include "Dungeons.h"
#include "MobEnchantmentComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "util/Algo.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "AbilitySystemGlobals.h"
#include "GrowComponent.h"
#include "game/Enchantments/Enchantment.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "world/entity/MobTags.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"

UEnchantedGameplayEffect::UEnchantedGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Enchanted"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchanted"), 0, 1);
}

UEnchantedHpBoostGameplayEffect::UEnchantedHpBoostGameplayEffect() {
	FAttributeBasedFloat HealthBonus;
	HealthBonus.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeBaseValue;
	HealthBonus.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	HealthBonus.BackingAttribute.bSnapshot = true;
	HealthBonus.Coefficient = BonusMaxHealthFraction;
	HealthBonus.PostMultiplyAdditiveValue = BonusMaxHealthAbsolute;

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = HealthBonus;
	healthModifier.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	Modifiers.Add(healthModifier);

	FGameplayModifierInfo staggerMultiplierModifier;
	staggerMultiplierModifier.ModifierOp = EGameplayModOp::Division;
	staggerMultiplierModifier.ModifierMagnitude = FScalableFloat(4.f);
	staggerMultiplierModifier.Attribute = UHealthAttributeSet::StaggerMultiplierAttribute();
	Modifiers.Add(staggerMultiplierModifier);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Enchanted"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchanted"), 0, 1);
}

UAncientEnchantedGameplayEffect::UAncientEnchantedGameplayEffect() {
	//Speed
	FGameplayModifierInfo speedModifier;
	speedModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	speedModifier.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	speedModifier.ModifierMagnitude = FScalableFloat(MovementSpeedMultiplier);
	Modifiers.Add(speedModifier);

	//Health
	FAttributeBasedFloat HealthBonus;
	HealthBonus.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeBaseValue;
	HealthBonus.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	HealthBonus.BackingAttribute.bSnapshot = true;
	HealthBonus.Coefficient = BonusMaxHealthFraction;
	HealthBonus.PostMultiplyAdditiveValue = BonusMaxHealthAbsolute;
	
	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = HealthBonus;
	healthModifier.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	Modifiers.Add(healthModifier);

	//Damage
	FGameplayModifierInfo meleeDamageMultiplier;
	meleeDamageMultiplier.ModifierOp = EGameplayModOp::Multiplicitive;
	meleeDamageMultiplier.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();
	meleeDamageMultiplier.ModifierMagnitude = FScalableFloat(DamageMultiplier);
	Modifiers.Add(meleeDamageMultiplier);

	FGameplayModifierInfo rangedDamageMultiplier;
	rangedDamageMultiplier.ModifierOp = EGameplayModOp::Multiplicitive;
	rangedDamageMultiplier.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();
	rangedDamageMultiplier.ModifierMagnitude = FScalableFloat(DamageMultiplier);
	Modifiers.Add(rangedDamageMultiplier);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Immunity.Stun")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
	
	
	GameplayCues.Empty();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchanted.Ancient"), 0, 1);
}

void UMobEnchantmentComponent::OnEnchanted(const FGameplayTag tag, int32 tagCount) {
	auto owner = GetOwnerMobCharacter();
	auto growComponent = owner->FindComponentByClass<UGrowComponent>();

	if (tagCount > 0) {
		if (!growComponent) {
			growComponent = NewObject<UGrowComponent>(owner, UGrowComponent::StaticClass());
			growComponent->RegisterComponent();
			growComponent->SetIsReplicated(true);
		}
		//Not the best place to do this, but hey, I got no better suggestion.
		if (owner->HasTag(MobTags::HashTag_Ancient) && growComponent->GetMaxScale() < AncientScaleMultiplier) {
			growComponent->SetMaxScale(AncientScaleMultiplier);
		}
		growComponent->Inflate();
	}
	else if(growComponent) {
		growComponent->Deflate();
	}
}

AMobCharacter * UMobEnchantmentComponent::GetOwnerMobCharacter() const
{
	return Cast<AMobCharacter>(GetOwner());
}

void UMobEnchantmentComponent::BeginPlay() {
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		auto delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UMobEnchantmentComponent::OnEnchanted);
		abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Enchanted")), delegate, EGameplayTagEventType::NewOrRemoved);
	}
}

bool UMobEnchantmentComponent::IsEnchanted() const {
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	return abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Enchanted"));
}

TArray<FEnchantmentData> UMobEnchantmentComponent::GetEnchantmentsNonInherent() const {
	TArray<FEnchantmentData> enchantments;

	for (const auto entry : Enchantments) {
		if (entry != nullptr && !entry->TreatAsInherrent) {
			enchantments.Emplace(entry->GetTypeId(), entry->GetLevel());
		}
	}

	return enchantments;
}

void UMobEnchantmentComponent::OnEnchantmentRegistered(UEnchantment* enchant) {
	if(!enchant->TreatAsInherrent) {
		enchant->bAlwaysTrigger = true;
	}
}

void UMobEnchantmentComponent::OnRep_Enchantments() {
	Super::OnRep_Enchantments();
	if (GetOwnerRole() != ROLE_Authority) {
		return;
	}

	bool hasHpBoostedEnchantment = false;
	int levels = 0;

	for (const auto* enchantment : Enchantments) {
		const auto type = enchantment->GetType();
		if (!enchantment->TreatAsInherrent) {
			hasHpBoostedEnchantment = hasHpBoostedEnchantment || enchantment->GivesHpBoost();
			levels++;
		}
	}

	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

	if (handle.IsValid()) {
		abilitySystem->RemoveActiveGameplayEffect(handle);
	}

	if (levels > 0) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
		auto owner = GetOwnerMobCharacter();

		if (owner->HasTag(MobTags::HashTag_Ancient)) {
			handle = abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UAncientEnchantedGameplayEffect>(abilitySystem, levels));
		}
		else {

			const auto spec = hasHpBoostedEnchantment ?
				effects::CreateGameplayEffectSpec<UEnchantedHpBoostGameplayEffect>(abilitySystem, levels) :
				effects::CreateGameplayEffectSpec<UEnchantedGameplayEffect>(abilitySystem, levels);

			handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}