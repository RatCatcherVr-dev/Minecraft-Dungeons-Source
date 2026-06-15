#include "Dungeons.h"
#include "StaggerComponent.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UStaggerComponent::UStaggerComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	IgnoredDamageTypes.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")));
	IgnoredDamageTypes.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));
}

void UStaggerComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = GetAbilitySystem();
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::EnduranceAttribute()).AddUObject(this, &UStaggerComponent::OnAttributeEnduranceChanged);
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::MaxEnduranceAttribute()).AddUObject(this, &UStaggerComponent::OnAttributeMaxEnduranceChanged);

		if (Duration <= 0) {
			abilitySystem->ApplyGameplayEffectToSelf(Cast<const UGameplayEffect>(UStunImmunityGameplayEffect::StaticClass()->GetDefaultObject()), 1.0f, abilitySystem->MakeEffectContext());
		}

		abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::StaggerMultiplierAttribute(), Multiplier);
		abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::EnduranceRecoveryPerSecondAttribute(), RecoverPerSecond);
	}
}

void UStaggerComponent::Recover() {
	auto abilitySystem = GetAbilitySystem();
	abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::EnduranceAttribute(), abilitySystem->GetNumericAttribute(UHealthAttributeSet::MaxEnduranceAttribute()));
}

void UStaggerComponent::OnAttributeEnduranceChanged(const FOnAttributeChangeData& data) {
	auto abilitySystem = GetAbilitySystem();
	if (data.NewValue <= 0.f) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UStaggerGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(effects::DurationName, Duration);
		GetAbilitySystem()->ApplyGameplayEffectSpecToSelf(spec);
		//Defer recovery one frame to avoid super deep callstack.
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UStaggerComponent::Recover);
	}
	else if(data.NewValue < abilitySystem->GetNumericAttribute(UHealthAttributeSet::MaxEnduranceAttribute())) {
		if (!RecoveryHandle.IsValid()) {
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UEnduranceRecoveryGameplayEffect>(abilitySystem, 1.f);
			spec.Period = 0.1f;			
			RecoveryHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
	else if (RecoveryHandle.IsValid()) {
		abilitySystem->RemoveActiveGameplayEffect(RecoveryHandle);
		RecoveryHandle.Invalidate();
	}
}

void UStaggerComponent::OnAttributeMaxEnduranceChanged(const FOnAttributeChangeData& data) {
	auto abilitySystem = GetAbilitySystem();
	const float currentEndurance = abilitySystem->GetNumericAttribute(UHealthAttributeSet::EnduranceAttribute());
	const float normalizedEndurance = FMath::RoundToFloat(FMath::Clamp(currentEndurance / data.OldValue, 0.f, 1.f));

	abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::EnduranceAttribute(), data.NewValue * normalizedEndurance);
}


UAbilitySystemComponent* UStaggerComponent::GetAbilitySystem() const {
	ensure(GetOwner()->IsA<ABaseCharacter>());
	return Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();
}