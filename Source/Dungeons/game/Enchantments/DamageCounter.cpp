#include "Dungeons.h"
#include "DamageCounter.h"
#include <GameplayEffect.h>
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"

const FName UDamageCounterGameplayEffect::EffectDurationName = FName("EffectDuration");

UDamageCounter::UDamageCounter() {
	TypeId = EEnchantmentTypeID::DamageCounter;
}

void UDamageCounter::BeginPlay()
{
	Super::BeginPlay();

	abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();

	abilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UDamageCounter::HandleGameplayEffectAdded);

	maxHealth = GetCharacterOwner()->GetHealthComponent()->GetMaximumHealth();
}

void UDamageCounter::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	
	FGameplayTagContainer damageTags;
	data.EffectSpec.GetAllAssetTags(damageTags);

	if (damageTags.HasTag(damageTypeToTrack)) {
		if (data.EvaluatedData.Magnitude < 0) {
			damageArray.Add(data.EvaluatedData.Magnitude);
		}

		auto spec = effects::CreateGameplayEffectSpec<UDamageCounterGameplayEffect>(abilitySystem, 1.0f);
		spec.SetSetByCallerMagnitude(UDamageCounterGameplayEffect::EffectDurationName, effectDuration);
		handle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, abilitySystem);

		if (GetOwnerRole() == ROLE_Authority)
			BroadcastEnchantmentTriggeredEvent();
	}
}

void UDamageCounter::OnCountChanged(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 PreviousCount)
{
	if (NewCount < PreviousCount) {
		int countToRemove = PreviousCount - NewCount;
		if (damageArray.Num() >= countToRemove)
		{
			damageArray.RemoveAt(0, countToRemove);
		}
	}

	cumulativeDamage = 0.0f;
	for (int i = 0; i < damageArray.Num(); i++)	{
		cumulativeDamage -= damageArray[i];		
	}	

	float healthPercentageLost = cumulativeDamage / maxHealth;
	hasReachedTarget = healthPercentageLost > healthLossTriggerThreshold;
}

bool UDamageCounter::HasReachedTarget()
{
	return hasReachedTarget;
}

void UDamageCounter::RemoveStack()
{
	abilitySystem->RemoveActiveGameplayEffect(handle);
	hasReachedTarget = false;
}

void UDamageCounter::HandleGameplayEffectAdded(UAbilitySystemComponent* abilitySystemComp, const FGameplayEffectSpec& effectspec, FActiveGameplayEffectHandle effectHandle)
{
	abilitySystemComp->OnGameplayEffectStackChangeDelegate(effectHandle)->AddUObject(this, &UDamageCounter::OnCountChanged);
}

UDamageCounterGameplayEffect::UDamageCounterGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = EffectDurationName;
	
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration;
	DurationMagnitude = durationMagnitude;

	bExecutePeriodicEffectOnApplication = false;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DamageCounter"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DamageCounter"));
}
