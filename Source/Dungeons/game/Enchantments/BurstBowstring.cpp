// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "BurstBowstring.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/util/ValueFormat.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/component/RangedAttackComponent.h"
#include "DungeonsGameMode.h"
#include "util/RandomUtil.h"
#include "game/item/ItemSlot.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "game/item/ArrowItemSlot.h"


UBurstBowstring::UBurstBowstring() {
	TypeId = EEnchantmentTypeID::BurstBowstring;
	PredictiveExecution = false;
	LevelMultiplier = [this](int level) -> float {
		return AttacksAtLevelOne + (level - 1) * AdditionalAttacksPerLevel;
	};

	MultiplierFormatter = valueformat::asConstant;
}

const FName URollingFortressGameplayEffect::DamageReductionKey(TEXT("DamageReduction"));

URollingFortressGameplayEffect::URollingFortressGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = URollingFortressGameplayEffect::DamageReductionKey;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(dealDamageInfo);
}


FText UBurstBowstring::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), valueformat::getAsWordOrNumber(1), FText::FromString(valueformat::asPercentage(DamageMultiplier)));
}

void UBurstBowstring::OnDeath() {
	EndAttack();
}

void UBurstBowstring::OnStart() {
	Super::OnStart();
	if (auto owner = Cast<ABaseCharacter>(GetOwner())) {
		owner->OnDeath.AddUObject(this, &UBurstBowstring::OnDeath);
	}
}

void UBurstBowstring::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	auto* owner = GetCharacterOwner();
	auto* abilitySystem = owner->GetAbilitySystemComponent();
	auto* rangedAttackComponent = GetOwner()->FindComponentByClass<URangedAttackComponent>();
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<URollingFortressGameplayEffect>(abilitySystem, Level);
	spec.SetSetByCallerMagnitude(URollingFortressGameplayEffect::DamageReductionKey, 1.f / DamageMultiplier);
	EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, key);
	UArrowItemSlot* slot = Cast<UArrowItemSlot>(rangedAttackComponent->GetCurrentAmmoSlot());

	if (owner->HasAuthority() && slot && !slot->IsRangedOverride() && slot->GetCount() > 0) {
		owner->GetHealthComponent()->OnDeath.AddUObject(this, &UBurstBowstring::EndAttack);
		bShouldTriggerCue = true;
		bHasSuccesfullyAttacked = false;
		rangedAttackComponent->ToggleAlwaysAttack(true);
		const auto hostile = characterquery::is::hostile(owner);
		const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

		TArray<TWeakObjectPtr<ABaseCharacter>> potentialTargets = algo::map_tarray(actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(owner, TargetGatheringRadius).FilterByPredicate(predicate), RETLAMBDA(TWeakObjectPtr<ABaseCharacter>(it)));
		TriggerAttack(FMath::RoundFromZero(LevelMultiplier(Level)), potentialTargets);
	}
}

void UBurstBowstring::OnDodgeRollEnd(FPredictionKey) {
	auto* owner = GetCharacterOwner();

	if (owner->HasAuthority()) {
		if (AttackTriggerHandle.IsValid()) {
			AttackTriggerHandle.Invalidate();
			EndAttack();
		}
	}
}

void UBurstBowstring::OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey key) {
	if (bShouldTriggerCue) {
		auto* owner = GetCharacterOwner();
		auto* abilitySystem = owner->GetAbilitySystemComponent();
		auto* rangedAttack = owner->FindComponentByClass<URangedAttackComponent>();
		Swap(abilitySystem->ScopedPredictionKey, key);
		FGameplayCueParameters params;
		params.SourceObject = rangedAttack->GetCurrentWeapon();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Enchantment.RollingFortress")), params);
		Swap(abilitySystem->ScopedPredictionKey, key);
	}
}

void UBurstBowstring::EndAttack() {

	auto* owner = GetCharacterOwner();
	bShouldTriggerCue = false;
	auto* rangedAttackComponent = GetOwner()->FindComponentByClass<URangedAttackComponent>();
	owner->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(EffectHandle);
	rangedAttackComponent->ToggleAlwaysAttack(false);
	owner->GetHealthComponent()->OnDeath.RemoveAll(this);
	
	//We only consume arrows if we actually fired at least once.
	if (bHasSuccesfullyAttacked) {
		rangedAttackComponent->ConsumeAmmo(nullptr);
	}
}

void UBurstBowstring::TriggerAttack(int RemainingAttacks, TArray<TWeakObjectPtr<ABaseCharacter>> PotentialTargets) {
	ABaseCharacter* target = nullptr;
	AttackTriggerHandle.Invalidate();

	while (PotentialTargets.Num() && target == nullptr) {
		PotentialTargets.Swap(PotentialTargets.Num() - 1, FMath::RandHelper(PotentialTargets.Num()));
		target = PotentialTargets.Pop().Get();
	}

	if (target) {
		if (!bHasSuccesfullyAttacked) {
			BroadcastEnchantmentTriggeredEvent();
			bHasSuccesfullyAttacked = true;
		}
		auto* rangedAttackComponent = GetOwner()->FindComponentByClass<URangedAttackComponent>();
		
		rangedAttackComponent->ToggleCharged(true);
		rangedAttackComponent->ToggleShouldConsumeAmmo(false);
		rangedAttackComponent->AttackLocal(target);
		rangedAttackComponent->ToggleShouldConsumeAmmo(true);

		if (--RemainingAttacks) {
			GetWorld()->GetTimerManager().SetTimer(AttackTriggerHandle, FTimerDelegate::CreateUObject(this, &UBurstBowstring::TriggerAttack, RemainingAttacks, PotentialTargets), TimeBetweenAttacks, false);
		} else {
			EndAttack();
		}

	}
	else {
		EndAttack();
	}

}