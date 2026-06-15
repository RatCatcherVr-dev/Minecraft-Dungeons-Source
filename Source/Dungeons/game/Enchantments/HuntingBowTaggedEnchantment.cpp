#include "Dungeons.h"
#include "HuntingBowTaggedEnchantment.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "GameplayEffectExtension.h"
#include <AbilitySystemComponent.h>

UTaggedGameplayEffect::UTaggedGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = false;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Tagged.HuntingBow")), 0, 1);
}

void UHuntingBowTaggedEnchantment::OnStart() {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) {
		return;
	}

	// apply gameplay effect
	if (ABaseCharacter* character = Cast<ABaseCharacter>(owner)) {
		const UTaggedGameplayEffect* effect = Cast<UTaggedGameplayEffect>(UTaggedGameplayEffect::StaticClass()->GetDefaultObject());
		UAbilitySystemComponent* abilitySystem = character->GetAbilitySystemComponent();
		// add to object
		Handle = abilitySystem->ApplyGameplayEffectToSelf(effect, 1, abilitySystem->MakeEffectContext());
	}
}

void UHuntingBowTaggedEnchantment::OnEnd() {
	//remove using handle
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) {
		return;
	}

	// apply gameplay effect
	if (ABaseCharacter* character = Cast<ABaseCharacter>(owner)) {
		UAbilitySystemComponent* abilitySystem = character->GetAbilitySystemComponent();
		// add to object
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

UHuntingBowTaggedEnchantment::UHuntingBowTaggedEnchantment() {
	TypeId = EEnchantmentTypeID::HuntingBowTaggedEnchantment;
}

void UHuntingBowTaggedEnchantment::OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData &data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) {
	if (GetOwnerRole() != ROLE_Authority) return;

	auto characterOwner = GetCharacterOwner();
	ABaseCharacter* byWhom = Cast<ABaseCharacter>(data.EffectSpec.GetContext().GetInstigator());
	if (byWhom && byWhom->IsFriendlyTowards(characterOwner) && byWhom != characterOwner) {
		data.EvaluatedData.Magnitude *= 1.25;
	}
}

void UHuntingBowTaggedEnchantment::SetTaggedByCharacter(ABaseCharacter* taggedBy) {
	TaggedByCharacter = taggedBy;
}
