#include "Dungeons.h"
#include "ProjectileCounter.h"
#include <GameplayEffect.h>
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/Enchantments/Dynamo.h"

const FName URangedHitGameplayEffect::EffectDurationName = FName("EffectDuration");

UProjectileCounter::UProjectileCounter() {
	TypeId = EEnchantmentTypeID::ProjectileCounter;
}

void UProjectileCounter::BeginPlay()
{
	Super::BeginPlay();

	abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();

	auto delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UProjectileCounter::OnCountChanged);
	abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.ProjectileHit"), delegate, EGameplayTagEventType::AnyCountChange);

}

void UProjectileCounter::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	
	FGameplayTagContainer damageTags;
	data.EffectSpec.GetAllAssetTags(damageTags);

	if (damageTags.HasTag(FGameplayTag::RequestGameplayTag("Damage.Ranged")))
	{
		auto spec = effects::CreateGameplayEffectSpec<URangedHitGameplayEffect>(abilitySystem, 1.0f);
		spec.SetSetByCallerMagnitude(URangedHitGameplayEffect::EffectDurationName, effectDuration);
		handle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, abilitySystem);

		if (GetOwnerRole() == ROLE_Authority)
			BroadcastEnchantmentTriggeredEvent();
	}
}

void UProjectileCounter::OnCountChanged(const FGameplayTag Tag, int32 Count)
{
	int tagCount = abilitySystem->GetCurrentStackCount(handle);
	hasReachedTarget = tagCount >= targetCount;
}

bool UProjectileCounter::HasReachedTarget()
{
	return hasReachedTarget;
}

void UProjectileCounter::RemoveStack()
{
	abilitySystem->RemoveActiveGameplayEffect(handle);
	hasReachedTarget = false;
}

URangedHitGameplayEffect::URangedHitGameplayEffect()
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

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.ProjectileHit"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.ProjectileHit"));
}
