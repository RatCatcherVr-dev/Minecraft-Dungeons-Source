#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "ThriveUnderPressure.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/util/ActorQuery.h"

namespace {
	FName ThriveUnderPressureTakeDamageEffectMagnitude(TEXT("ThriveUnderPressureTakeDamageMagnitude"));
}

UThriveUnderPressure::UThriveUnderPressure() {
	Effect = UThriveUnderPressureGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::ThriveUnderPressure;

	LevelMultiplier = [this](int level) -> float {
		return 1.0f + (0.06f * level);
	};
	MultiplierFormatter = valueformat::asRelativeDividerPercentageChange;
}

void UThriveUnderPressure::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (effectOn) {
		GetCharacterOwner()->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(Handle);
		effectOn = false;
	}

	Super::EndPlay(EndPlayReason);
}

void UThriveUnderPressure::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (GetOwnerRole() != ROLE_Authority)
		return;


	int enemyCount = 0;
	for (auto mob : actorquery::getNearbyActors<AMobCharacter>(GetOwner(), TriggerRange)) {
		if (!mob->IsFriendlyTowardsPlayers() && mob->IsAlive())
			enemyCount++;
	}

	if (enemyCount >= MobThreshold)
	{
		if (!effectOn)
		{
			auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
			FGameplayEffectSpec spec(Cast<UThriveUnderPressureGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
			spec.SetSetByCallerMagnitude(::ThriveUnderPressureTakeDamageEffectMagnitude, 2.0f);
			Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			effectOn = true;
		}
	}
	else
	{
		if (effectOn)
		{
			auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
			abilitySystem->RemoveActiveGameplayEffect(Handle);
			effectOn = false;
		}
	}
}

UThriveUnderPressureGameplayEffect::UThriveUnderPressureGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo takeDamageInfo;
	takeDamageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat takeDamageMagnitude;
	takeDamageMagnitude.DataName = ::ThriveUnderPressureTakeDamageEffectMagnitude;

	takeDamageInfo.ModifierMagnitude = takeDamageMagnitude;
	takeDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(takeDamageInfo);
}