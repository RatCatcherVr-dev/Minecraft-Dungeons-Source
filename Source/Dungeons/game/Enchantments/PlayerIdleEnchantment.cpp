#include "PlayerIdleEnchantment.h"

// ---------- Gameplay Effects ---------- //

UPlayerIdleGameplayEffect::UPlayerIdleGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Idle"), 0, 1);
}

UBardGarbIdleGameplayEffect::UBardGarbIdleGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Empty();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Idle.BardGarb"), 0, 1);
}

UBardGarbUnique1IdleGameplayEffect::UBardGarbUnique1IdleGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Empty();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Idle.BardGarb.Unique1"), 0, 1);
}


// ---------- Enchantments ---------- //

UPlayerIdleEnchantment::UPlayerIdleEnchantment() {
	Effect = UPlayerIdleGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::PlayerIdle;
}

void UPlayerIdleEnchantment::EndPlay(EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);
	RemoveIdleEffect();
}

void UPlayerIdleEnchantment::RemoveIdleEffect() {
	if (Handle.IsValid()) {
		if (auto character = Cast<APlayerCharacter>(GetOwner())) {
			if (auto abilitySystem = character->GetAbilitySystemComponent()) {
				abilitySystem->RemoveActiveGameplayEffect(Handle);
			}
		}
	}
}

void UPlayerIdleEnchantment::OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle) {
	if (idle == EPlayerIdleState::Idle) {
		if (auto character = Cast<ABaseCharacter>(GetOwner())) {
			if (auto abilitySystem = character->GetAbilitySystemComponent()) {
				Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(FGameplayEffectSpec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), 1));
			}
		}
	}
	else {
		RemoveIdleEffect();
	}
}

UBardGarbIdle::UBardGarbIdle() {
	Effect = UBardGarbIdleGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::BardIdle;
}

UBardGarbUnique1Idle::UBardGarbUnique1Idle() {
	Effect = UBardGarbUnique1IdleGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::BardUnique1Idle;
}
