#include "Dungeons.h"
#include "SlowImmunity.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>

// ----- SLOW IMMUNITY GAMEPLAY EFFECT -----//
USlowImmunityGameplayEffect::USlowImmunityGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Slow"));

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Slow"));
}

// ----- SLOW IMMUNITY ENCHANTMENT ----- //
USlowImmunity::USlowImmunity() {
	TypeId = EEnchantmentTypeID::SlowImmunity;
}

void USlowImmunity::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USlowImmunityGameplayEffect>(abilitySystem, 1.f);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void USlowImmunity::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
