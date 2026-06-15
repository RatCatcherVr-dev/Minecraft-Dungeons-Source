#include "Dungeons.h"
#include "SlowbowEnchantment.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/component/EnchantmentComponent.h"
#include "game/util/Tags.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"

USlowBowEnchantment::USlowBowEnchantment() {
	TypeId = EEnchantmentTypeID::SlowBowEnchantment;
	mGameplayEffect = UFreezingGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return mInitialFreezeAmount - mFreezePerLevel * (level - 1);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void USlowBowEnchantment::OnProjectileLaunch(ABaseProjectile* fromProjectile) {
	if (ABaseCharacter* character = Cast<ABaseCharacter>(GetOwner())) {
		auto abilitySystem = character->GetAbilitySystemComponent();

		const float SlowMultiplier = LevelMultiplier(Level);
		const float SlowDivider = 1 / SlowMultiplier;

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, mGameplayEffect, Level);
		spec.SetSetByCallerMagnitude(USlowMultiplicativeResistanceModCalculation::SetByCallerKey, SlowDivider);
		spec.SetSetByCallerMagnitude(effects::DurationName, mDuration);

		fromProjectile->GameplaySpecsToApply.Add(spec);
	}
}
