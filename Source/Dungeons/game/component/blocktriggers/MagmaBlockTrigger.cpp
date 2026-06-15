#include "Dungeons.h" 
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/LavaGameplayEffect.h"
#include "game/abilities/effects/LavaBurningGamePlayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "MagmaBlockTrigger.h"

#include "game/Conversion.h"

void UMagmaBlockTrigger::OnEnterStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<ULavaGameplayEffect>(ULavaGameplayEffect::StaticClass()->GetDefaultObject()), character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
	character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(spec);
}

void UMagmaBlockTrigger::OnExitStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const {
	FGameplayTagContainer tags;
	tags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Fire.Lava"));
	if (const UAbilitySystemComponent* abilitySystem = character->GetAbilitySystemComponent()) {
		character->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(tags);
	}
}

bool UMagmaBlockTrigger::IsTrigger(const FullBlock& fullBlock, ABaseCharacter* const character) const {
	const Block& block = fullBlock.getBlock();
	if (block.getId() == Block::mCustom5->getId() || // magma
		block.getId() == Block::mGlowStone->getId() ||
		block.getId() == Block::mEmeraldBlock->getId()) {
		return true;
	}
	return false;
}