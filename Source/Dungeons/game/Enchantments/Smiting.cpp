#include "Dungeons.h"
#include "Smiting.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "world/entity/MobTags.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

USmiting::USmiting() {
	TypeId = EEnchantmentTypeID::Smiting;
	PredictiveExecution = true;
	LevelMultiplier = [this](int level) -> float {
		return DamageMultiplierBase + DamageMultiplierPerLevel * level;
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void USmiting::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}

	if (auto mob = Cast<AMobCharacter>(targetComponent->GetAvatarActor())) {
		if (hasMobTag(mob->EntityType, MobTags::HashTag_Undead)) {
			float currentDamage = mutableSpec.GetSetByCallerMagnitude(effects::HealthName);
			currentDamage *= LevelMultiplier(Level);
			mutableSpec.SetSetByCallerMagnitude(effects::HealthName, currentDamage);
			mutableSpec.DynamicAssetTags.AddTag(SmitingDamageType);
			effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Smiting.Melee")));

			if (GetOwnerRole() == ROLE_Authority) {
				BroadcastEnchantmentTriggeredEvent();
			}
		}
	}
}

void USmiting::OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}

	if (auto mob = Cast<AMobCharacter>(toWhom)) {
		if (hasMobTag(mob->EntityType, MobTags::HashTag_Undead)) {

			outDamage *= LevelMultiplier(Level);
			damageType = SmitingDamageType;

			auto abilitySystem = Cast<ABaseCharacter>(toWhat)->GetAbilitySystemComponent();
			FGameplayCueParameters params;
			params.Location = toWhat->GetActorLocation();
			params.Instigator = GetOwner();
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Smiting"), params);
			
			if (GetOwnerRole() == ROLE_Authority) {
				BroadcastEnchantmentTriggeredEvent();
			}
		}
	}
}
