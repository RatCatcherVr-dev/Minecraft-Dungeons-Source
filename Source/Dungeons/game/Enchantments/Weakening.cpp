#include "Dungeons.h"
#include "Weakening.h"
#include "game/actor/character/mob/MobCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "util/CharacterQuery.h"

UWeakening::UWeakening() {
	TypeId = EEnchantmentTypeID::Weakening;
	PredictiveExecution = true;
	Effect = UDamageWeakeningGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return WeakenAmount[FMath::Min(WeakenAmount.Num() - 1, level-1)];
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}


FText UWeakening::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(Duration)));
}

FGameplayEffectSpec UWeakening::CreateWeakenEffectSpec(UAbilitySystemComponent* abilitySystem) const {
	auto spec = FGameplayEffectSpec(Cast<UDamageWeakeningGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
	const auto dealDamageEffect = IsOwnerMob() ? MobWeakenAmount : LevelMultiplier(Level);
	spec.SetSetByCallerMagnitude(UDamageWeakeningGameplayEffect::DurationName, IsOwnerMob() ? MobDuration : Duration);
	spec.SetSetByCallerMagnitude(UDamageWeakeningGameplayEffect::DamageDealtDividerName, 1.f/dealDamageEffect);
	return spec;
}

TArray<ABaseCharacter*> UWeakening::GetNearbyTargets(ABaseCharacter* characterTarget, const ABaseCharacter* characterOwner) {
	auto potentialTargets = actorquery::getNearbyActors<ABaseCharacter>(characterTarget, WeakenRange);

	const auto predicate = [=](const ABaseCharacter* potentialTarget) {
		return characterquery::is::targetable(potentialTarget) && !characterOwner->IsFriendlyTowards(potentialTarget) && potentialTarget->IsAlive();
	};
	auto targets = potentialTargets.FilterByPredicate(predicate);
	
	targets.Add(characterTarget);
	
	return targets;
}

bool UWeakening::ApplyDebuff(ABaseCharacter* target, FSharedPredictionContext context) {

	if (target && characterquery::is::targetable(target) && actorquery::is::alive(target))	{
		if (GetOwnerRole() == ROLE_Authority) {
			BroadcastEnchantmentTriggeredEvent();
		}

		const auto characterOwner = GetCharacterOwner();
		auto abilitySystem = characterOwner->GetAbilitySystemComponent();

		const auto spec = CreateWeakenEffectSpec(abilitySystem);

		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent(), context.GetKey());

		for (auto extraTarget : GetNearbyTargets(target, characterOwner)) {
			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, extraTarget->GetAbilitySystemComponent(), context.GetKey());
		}

		return true;
	}

	return false;
}

void UWeakening::OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}
	if(ApplyDebuff(Cast<ABaseCharacter>(targetComponent->GetAvatarActor()), context))	{
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		mutableSpec.DynamicAssetTags.AddTag(WeakeningDamageType);
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Weakening.Melee")));
	}
}

void UWeakening::OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	if(ApplyDebuff(Cast<ABaseCharacter>(toWhat), FSharedPredictionContext()))	{
		auto abilitySystem = Cast<ABaseCharacter>(toWhat)->GetAbilitySystemComponent();
		FGameplayCueParameters params;
		params.Location = toWhat->GetActorLocation();
		params.Instigator = GetOwner();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Weakening"), params);
	}
}

