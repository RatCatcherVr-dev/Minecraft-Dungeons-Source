#include "BaneEnchantment.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "world/entity/MobTags.h"

UBaneEnchantment::UBaneEnchantment() {
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaneMultiplier + (BanePercentagePerLevel * (level - 1));
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UBaneEnchantment::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) {
	if (auto mob = Cast<AMobCharacter>(targetComponent->GetAvatarActor())) {
		if (hasMobTag(mob->EntityType, BaneMobTag) || bAlwaysTrigger) {
			float currentDamage = mutableSpec.GetSetByCallerMagnitude(effects::HealthName);
			currentDamage *= LevelMultiplier(Level);
			mutableSpec.SetSetByCallerMagnitude(effects::HealthName, currentDamage);
			mutableSpec.DynamicAssetTags.AddTag(BaneType);
			effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Bane.Melee")));

			if (GetOwnerRole() == ROLE_Authority) {
				BroadcastEnchantmentTriggeredEvent();
			}
		}
	}
}

UBaneOfIllagers::UBaneOfIllagers()
	: Super()
{
	TypeId = EEnchantmentTypeID::BaneOfIllagers;
	BaneType = FGameplayTag::RequestGameplayTag("Damage.Bane.Illager");
	BaneMobTag = MobTags::HashTag_Illager;
}
