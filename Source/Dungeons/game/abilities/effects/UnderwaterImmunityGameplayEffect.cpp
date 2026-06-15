#include "UnderwaterImmunityGameplayEffect.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffectUtil.h"
#include "AbilitySystemComponent.h"

UUnderwaterImmunityGameplayEffect::UUnderwaterImmunityGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Underwater"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
}

UUnderwaterImmunity::UUnderwaterImmunity()
{
	TypeId = EEnchantmentTypeID::UnderwaterImmunity;
}

void UUnderwaterImmunity::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UUnderwaterImmunityGameplayEffect>(abilitySystem, 1.f);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UUnderwaterImmunity::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
