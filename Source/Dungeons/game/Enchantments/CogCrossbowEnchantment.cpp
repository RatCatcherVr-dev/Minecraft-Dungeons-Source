#include "Dungeons.h"
#include "CogCrossbowEnchantment.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "Enchantment.h"
#include <GameplayEffect.h>
#include <AbilitySystemComponent.h>
#include "game/component/RangedAttackComponent.h"
#include "game/abilities/ui/DungeonsGameplayEffectUIData.h"
#include <UnrealNetwork.h>
#include <Engine/Engine.h>

namespace cogcrossbow {
	const static int MaxStack = 5;
}

UCogCrossbowEnchantment::UCogCrossbowEnchantment() {
	TypeId = EEnchantmentTypeID::CogCrossbowEnchantment;
	ServerOnlyExecution = false;

	LevelMultiplier = [](int level) { return 1.f; };
	MultiplierFormatter = valueformat::asConstant;
}

void UCogCrossbowEnchantment::BeginPlay() {
	Super::BeginPlay();
	RangedAttack = GetOwner()->FindComponentByClass<URangedAttackComponent>();
	GetWorld()->GetTimerManager().SetTimer(ChargeHandle, this, &UCogCrossbowEnchantment::OnCharge, ChargeDelay, true);
}

void UCogCrossbowEnchantment::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	GetWorld()->GetTimerManager().ClearTimer(ChargeHandle);
	bCanAttack = true;
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		abilitySystem->RemoveActiveGameplayEffect(EffectHandle);
	}
}

void UCogCrossbowEnchantment::OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey key) {
	attackDenied = !bCanAttack;
}

void UCogCrossbowEnchantment::OnRangedAttackEnded(bool, bool) {
	UnCharge();
	GetWorld()->GetTimerManager().SetTimer(ChargeHandle, this, &UCogCrossbowEnchantment::OnCharge, ChargeDelay, true);
}

void UCogCrossbowEnchantment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCogCrossbowEnchantment, bCanAttack);
}

void UCogCrossbowEnchantment::OnCharge() {
	if (GetOwner()->HasAuthority()) {
		if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
			auto spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, EffectClass);
			spec.GetContext().AddInstigator(GetOwner(), GetOwner());
			spec.GetContext().AddOrigin(GetOwner()->GetActorLocation());
			EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			bCanAttack = true;
			OnRep_CanAttack();
		}
	}
}

void UCogCrossbowEnchantment::UnCharge() {
	if (GetOwner()->HasAuthority()) {
		if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
			abilitySystem->RemoveActiveGameplayEffect(EffectHandle, 1);
			auto active = abilitySystem->GetActiveGameplayEffect(EffectHandle);
			if (!active) {
				bCanAttack = false;
			}
		}
	}
}

void UCogCrossbowEnchantment::OnRep_CanAttack() {
	if (RangedAttack.IsValid()) {
		if (bCanAttack) {
			RangedAttack->StopAttack();
		}
	}
}

UCogCrossBowChargeGameplayEffect::UCogCrossBowChargeGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	StackLimitCount = cogcrossbow::MaxStack;

	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.CogCrossbow")), 0, 1);
}
