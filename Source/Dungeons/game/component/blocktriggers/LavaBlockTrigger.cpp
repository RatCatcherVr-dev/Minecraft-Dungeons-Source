#include "Dungeons.h" 
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/LavaGameplayEffect.h"
#include "game/abilities/effects/LavaBurningGamePlayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "LavaBlockTrigger.h"

#include "game/Conversion.h"

void ULavaBlockTrigger::OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<ULavaGameplayEffect>(ULavaGameplayEffect::StaticClass()->GetDefaultObject()), character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
	character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(spec);
}

void ULavaBlockTrigger::OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	FGameplayEffectQuery query;
	query.EffectDefinition = ULavaGameplayEffect::StaticClass();
	auto effects = character->GetAbilitySystemComponent()->GetActiveEffects(query);

	if (effects.Num()) {
		for (auto& effect : effects) {
			character->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(effect);
		}
		//Start Burning when you exit lava...
		FGameplayEffectSpec spec(Cast<ULavaBurningGamePlayEffect>(ULavaBurningGamePlayEffect::StaticClass()->GetDefaultObject()), character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		spec.SetSetByCallerMagnitude(effects::DurationName, 3.f);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(spec);
	}
}

bool ULavaBlockTrigger::IsTrigger(const FullBlock& block, ABaseCharacter* const character) const
{
	return BlockGraphicsHelper::getBlock(character->GetWorld(), conversion::ueToBlock(character->GetActorLocation()), block.getBlock().getId()).getMaterialType() == EMaterialTypeEnum::Lava;
}
