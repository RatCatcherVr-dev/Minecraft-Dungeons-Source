


#include "UEffectBasedCooldownProvider.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "util/Algo.h"
#include "abilities/effects/GameplayEffectUtil.h"

UAbilitySystemComponent* UEffectBasedCooldownProvider::GetAbilitySystem() const
{
	return AbilitySystemProvider->GetAbilitySystemComponent();
}

void UEffectBasedCooldownProvider::PostInitProperties()
{
	Super::PostInitProperties();
	AbilitySystemProvider = Cast<IAbilitySystemInterface>(GetOuter());
}

void UEffectBasedCooldownProvider::PostRename(UObject* OldOuter, const FName OldName)
{
	Super::PostRename(OldOuter, OldName);
	AbilitySystemProvider = Cast<IAbilitySystemInterface>(GetOuter());
}

UEffectBasedCooldownProvider::UEffectBasedCooldownProvider(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemProvider = Cast<IAbilitySystemInterface>(GetOuter());
}


bool UEffectBasedCooldownProvider::IsOnCooldown() const
{
 	return GetAbilitySystem()->GetTagCount(CooldownTag) > 0;
}

float UEffectBasedCooldownProvider::GetCooldownSecondsRemaining() const
{
	return GetCooldownSecondsRemainingAndDuration().Key;
}

float UEffectBasedCooldownProvider::GetCooldownFractionRemaining() const
{
	auto entry = GetCooldownSecondsRemainingAndDuration();
	
	return entry.Key / entry.Value;
}

float UEffectBasedCooldownProvider::GetCooldownSecondsDuration() const
{
	return GetCooldownSecondsRemainingAndDuration().Value;
}

TPair<float, float> UEffectBasedCooldownProvider::GetCooldownSecondsRemainingAndDuration() const {
	auto abilitySystem = GetAbilitySystem();

	FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag));
	auto durationAndRemainder = abilitySystem->GetActiveEffectsTimeRemainingAndDuration(Query);

	if (durationAndRemainder.Num() == 0) return TPair<float,float>(0.f, 0.f);
	TPair<float, float> nearestDurationLeft = *algo::max_element(durationAndRemainder, [](const auto& v1, const auto& v2) -> bool { return v1.Key < v2.Key; });
	// Time Remaining shouldn't be bigger than the duration neither be smaller than 0
	nearestDurationLeft.Key = FMath::Clamp(nearestDurationLeft.Key, 0.f, nearestDurationLeft.Value);
	return nearestDurationLeft;
}

void UEffectBasedCooldownProvider::TriggerCooldown(float seconds, FPredictionKey key)
{
	if(seconds <= 0) return;
	
 	auto abilitySystem = GetAbilitySystem();

	auto spec = effects::CreateGameplayEffectSpec<UCooldownGameplayEffect>(abilitySystem);
	spec.SetSetByCallerMagnitude(FName(TEXT("Duration")), seconds);

	TriggerCooldownWithSpec(spec);
}

void UEffectBasedCooldownProvider::ModifyCooldown(float offsetSeconds) {
	auto abilitySystem = GetAbilitySystem();

	if (!abilitySystem->IsOwnerActorAuthoritative()) return;
	FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag));

	auto effects = abilitySystem->GetActiveEffects(Query);

	for (auto& effect : effects) {
		abilitySystem->ModifyActiveEffectStartTime(effect, offsetSeconds);
	}

}

void UEffectBasedCooldownProvider::ResetCooldown()
{
	auto abilitySystem = GetAbilitySystem();
	FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag));

	abilitySystem->RemoveActiveEffects(Query);
}

void UEffectBasedCooldownProvider::TriggerCooldownWithSpec(FGameplayEffectSpec& spec, FPredictionKey key)
{
	spec.DynamicGrantedTags.AddTag(CooldownTag);
	GetAbilitySystem()->ApplyGameplayEffectSpecToSelf(spec, key);
}

UCooldownGameplayEffect::UCooldownGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));
	DurationMagnitude = durationMagnitude;
		
	StackingType = EGameplayEffectStackingType::None;
}
