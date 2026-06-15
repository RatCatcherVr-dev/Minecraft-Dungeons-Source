#include "Dungeons.h"
#include "DamageExecutionCalculation.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/DamageNumberBatchingComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/DamageHelpers.h"

void UDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	const auto& spec = ExecutionParams.GetOwningSpec();
	const auto targetAbilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();

	const auto damageAttribute = spec.GetModifiedAttribute(UHealthAttributeSet::HealthAttribute());
	
	if (targetAbilitySystem && damageAttribute && damageAttribute->TotalMagnitude < 0) {
		const auto playerSource = GetPlayerSource(spec);

		if (playerSource && IsTargetAlive(targetAbilitySystem, damageAttribute)) {
			ExecuteDamageNumberGameplayCue(spec, targetAbilitySystem, damageAttribute, playerSource);
		}

		
		if(damageAttribute->TotalMagnitude + targetAbilitySystem->GetNumericAttribute(UHealthAttributeSet::HealthAttribute()) > 0.f) {
			//Hack time! We are stripping the normal out of damage executions if target will not die to save data on the rpc!
			auto context = effects::GetDungeonsContextFromSpec(spec);
			context->Normal.Reset();
		}
	}
}

bool UDamageExecutionCalculation::IsTargetAlive(const UAbilitySystemComponent* targetAbilitySystem, const FGameplayEffectModifiedAttribute* damageAttribute) {
	return targetAbilitySystem->GetNumericAttribute(UHealthAttributeSet::HealthAttribute()) - damageAttribute->TotalMagnitude > 0.f;
}

APlayerCharacter* UDamageExecutionCalculation::GetPlayerSource(const FGameplayEffectSpec& spec) const {
	auto playerSource = Cast<APlayerCharacter>(spec.GetContext().GetInstigator());

	if (!playerSource) {
		playerSource = Cast<APlayerCharacter>(spec.GetContext().GetEffectCauser());
	}

	return playerSource;
}

void UDamageExecutionCalculation::ExecuteDamageNumberGameplayCue(const FGameplayEffectSpec& spec, UAbilitySystemComponent* targetAbilitySystem, const FGameplayEffectModifiedAttribute* damageAttribute, APlayerCharacter* playerSource) const {
	FGameplayTagContainer tags;
	spec.GetAllAssetTags(tags);
	playerSource->GetDamageNumberBatchingComponent()->AddDamageInstance(damageAttribute->TotalMagnitude, targetAbilitySystem->GetAvatarActor(), spec.GetContext().GetOrigin(), tags);
}
