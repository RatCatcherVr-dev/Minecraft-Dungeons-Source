#include "Dungeons.h"
#include "Stunning.h"
#include <AbilitySystemComponent.h>
#include "game/actor/character/BaseCharacter.h"
#include "game/component/StaggerComponent.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"


UStunning::UStunning() {
	TypeId = EEnchantmentTypeID::Stunning;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return level * ChanceToStunPerLevel;
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UStunning::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	
	if (randStream.FRand() < LevelMultiplier(Level) || bAlwaysTrigger) {
		if (auto toWhatCharacter = Cast<ABaseCharacter>(toWhat)){
			auto abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();
			if (auto targetAbilitySystem = toWhatCharacter->GetAbilitySystemComponent()){

				const auto ImmunityStun = FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Immunity.Stun"));
				if (!targetAbilitySystem->HasAnyMatchingGameplayTags(ImmunityStun))
				{
					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
					FGameplayEffectSpec spec(Cast<const UGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
					spec.SetSetByCallerMagnitude(effects::DurationName, DurationSeconds);

					auto handle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem, context.GetKey());

					if (GetOwnerRole() == ROLE_Authority && handle.WasSuccessfullyApplied() && !IsNotAlive(toWhat))
					{
						BroadcastEnchantmentTriggeredEvent();
					}
				}
			}
		}
	}
}

bool UStunning::IsNotAlive(AActor* ToWhat) const
{
	if (const auto BaseCharacter = Cast<ABaseCharacter>(ToWhat))
	{
		if (const auto HealthComponent = BaseCharacter->FindComponentByClass<UHealthComponent>())
		{
			return HealthComponent->IsNotAlive();
		}
	}
	return false;
}
