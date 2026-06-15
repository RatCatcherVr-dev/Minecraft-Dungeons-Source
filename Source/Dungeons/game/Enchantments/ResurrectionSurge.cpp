#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

#include "ResurrectionSurge.h"

UResurrectionSurgeGameplayEffect::UResurrectionSurgeGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;

	FSetByCallerFloat multiplierFloat;
	multiplierFloat.DataName = FName("ResurrectionSurgeDividerMagnitude");

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	healthInfo.ModifierMagnitude = multiplierFloat;
	healthInfo.ModifierOp = EGameplayModOp::Type::Division;
	Modifiers.Add(healthInfo);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Enchantment.ResurrectionSurge"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.ResurrectionSurge"),0,1);

}

UResurrectionSurge::UResurrectionSurge() {
	TypeId = EEnchantmentTypeID::ResurrectionSurge;

	LevelMultiplier = [this](int level) -> float {
		return SurgeMultiplier + SurgePerLevel * (level - 1);
	};

	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
}

void UResurrectionSurge::OnStart() {
	if (GetOwnerRole() != ROLE_Authority) return;

	auto characterOwner = GetCharacterOwner();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();

	if(auto healthComponent = characterOwner->GetHealthComponent()){
		float power = 1 + healthComponent->GetNumberOfTimesRessurected() * (LevelMultiplier(Level) - 1);

		if (power > 1) {
			auto spec = effects::CreateGameplayEffectSpec<UResurrectionSurgeGameplayEffect>(abilitySystem);
			spec.SetSetByCallerMagnitude(FName("ResurrectionSurgeDividerMagnitude"), 1 / power);

			handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

			BroadcastEnchantmentTriggeredEvent();
		}
	}
}

void UResurrectionSurge::OnEnd() {
	if (auto characterOwner = GetCharacterOwner()) {
		if(handle.IsValid()){
			if (auto abilitySystem = characterOwner->GetAbilitySystemComponent()) {
				abilitySystem->RemoveActiveGameplayEffect(handle);
			}
		}
	}
}
