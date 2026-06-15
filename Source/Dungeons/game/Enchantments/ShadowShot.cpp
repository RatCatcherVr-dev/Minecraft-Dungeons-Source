#include "Dungeons.h"
#include "ShadowShot.h"
#include "game/item/instance/BackstabbersBrewInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/RangedAttackComponent.h"


UShadowShot::UShadowShot()
{
	TypeId = EEnchantmentTypeID::ShadowShot;
	PredictiveExecution = true;
}

 void UShadowShot::OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey key)
 {
	 auto ownerCharacter = GetCharacterOwner();

	 if (!ownerCharacter) return;

	 UAbilitySystemComponent* abilitySystem = ownerCharacter->GetAbilitySystemComponent();

	 const auto invisibleTag = FGameplayTag::RequestGameplayTag("StatusEffect.Invisible");
	 if (abilitySystem)
	 {
		 bool isInvisible = abilitySystem->HasMatchingGameplayTag(invisibleTag);
		 bTriggerShadowShot = !isInvisible; //shadow shot only triggers if we're not invisible
		 if (auto rangedAttackComponent = GetOwner()->FindComponentByClass<URangedAttackComponent>()) {
			 rangedAttackComponent->bRemoveInvisibilityAfterAttack = isInvisible; //don't remove invisibility if we gained it during this attack
		 }
	 }
 }


void UShadowShot::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream)
{
	const AMobCharacter* mob = Cast<AMobCharacter>(toWhat);
	auto ownerCharacter = GetCharacterOwner();

	if ((!bTriggerShadowShot && !bAlwaysTrigger) || (randStream.FRand() > TriggerChance && !bAlwaysTrigger) || !mob || !ownerCharacter || actorquery::is::alive(mob)) return;

	UAbilitySystemComponent* abilitySystem = ownerCharacter->GetAbilitySystemComponent();

	const auto invisibleTag = FGameplayTag::RequestGameplayTag("StatusEffect.Invisible");
	if (abilitySystem->HasMatchingGameplayTag(invisibleTag)) return;

	const auto effect = Cast<UBackstabbersBrewGameplayEffect>(Effect->GetDefaultObject());
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), Level);

	const float EffectDuration = DurationMagnitude*LevelMultiplier(Level);
	spec.SetSetByCallerMagnitude(effects::DurationName, EffectDuration);
	spec.SetSetByCallerMagnitude(TEXT("Magnitude"), MeleePowerBoostAmount);

	abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	ownerCharacter->OnInvisibilityGained();

	if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
}

