#include "Dungeons.h"
#include "Huge.h"
#include "game/component/GrowComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UHuge::UHuge() {
	PrimaryComponentTick.bCanEverTick = false;
	TypeId = EEnchantmentTypeID::Huge;
}

void UHuge::OnStart() {
	Super::OnStart();

	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	auto owner = GetOwner();
	GrowComponent = owner->FindComponentByClass<UGrowComponent>();

	if (!GrowComponent) {
		GrowComponent = NewObject<UGrowComponent>(owner, UGrowComponent::StaticClass());
		GrowComponent->RegisterComponent();
		GrowComponent->SetIsReplicated(true);
	}

	CachedStartScale = GrowComponent->GetMaxScale();
	GrowComponent->SetMaxScale(Scale);
	GrowComponent->Inflate();

	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		const auto spec = effects::CreateGameplayEffectSpec<UHugeGameplayEffect>(abilitySystem);
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UHuge::OnEnd() {
	Super::OnEnd();

	if (GetOwnerRole() == ROLE_Authority && GrowComponent) {
		GrowComponent->SetMaxScale(CachedStartScale);
		GrowComponent->Deflate();
	}
}

UHugeGameplayEffect::UHugeGameplayEffect() {
	FAttributeBasedFloat HealthBonus;
	HealthBonus.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeBaseValue;
	HealthBonus.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	HealthBonus.BackingAttribute.bSnapshot = true;
	HealthBonus.Coefficient = MaxHPMultiplier;
	HealthBonus.PostMultiplyAdditiveValue = AddedAbsoluteHP;

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = HealthBonus;
	healthModifier.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	Modifiers.Add(healthModifier);
}