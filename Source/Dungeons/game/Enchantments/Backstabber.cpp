#include "Dungeons.h"
#include "Backstabber.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "../actor/character/mob/MobCharacter.h"

UBackstabber::UBackstabber() {
	TypeId = EEnchantmentTypeID::Backstabber;
	PredictiveExecution = true;
}

void UBackstabber::PostInitProperties()
{
	Super::PostInitProperties();

	LevelMultiplier = [dmgMultiplier = DamageMultiplier](int level) { return dmgMultiplier * level; };
	MultiplierFormatter = [](float multiplier) { return valueformat::asConstant(FMath::RoundToInt(multiplier * 100.f)); };
}

FText UBackstabber::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRoundedMultiple(DamageMultiplier)));
}

void UBackstabber::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	bool bTriggerBackstab = false;
	if (auto mobTarget = Cast<AMobCharacter>(targetComponent->GetAvatarActor()))
	{
		{
			if (auto abilitySystem = mobTarget->GetAbilitySystemComponent())
			{
				//trigger due to defender's tags
				bTriggerBackstab |= abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Blind"));
				bTriggerBackstab |= abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Stunned"));
				bTriggerBackstab |= abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
			}

			if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner()))
			{
				if (auto abilitySystem = owner->GetAbilitySystemComponent())
				{
					//trigger due attacker's tags
					bTriggerBackstab |= abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Invisible"));
					bTriggerBackstab |= abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility"));
				}

				//trigger if not being targeted 
				bTriggerBackstab |= mobTarget->MobParams.target.actor.Get() != owner;
			}
		}
	}
	
	
	static const auto criticalTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Melee.Critical"));
	if (bAlwaysTrigger || bTriggerBackstab) 
	{
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		mutableSpec.DynamicAssetTags.AddTag(criticalTag);//for red damage numbers
		mutableSpec.SetSetByCallerMagnitude(effects::HealthName, mutableSpec.GetSetByCallerMagnitude(effects::HealthName) * (1 + LevelMultiplier(Level)));

		FGameplayCueParameters params;
		params.AbilityLevel = Level;

		targetComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.hit.Backstabber")), params);
	}
}