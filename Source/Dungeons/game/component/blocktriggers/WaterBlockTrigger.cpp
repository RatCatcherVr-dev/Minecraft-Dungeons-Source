#include "Dungeons.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/WaterGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "WaterBlockTrigger.h"

#include "game/Conversion.h"

void UWaterBlockTrigger::OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	FGameplayEffectSpec spec(Cast<UWaterGameplayEffect>(UWaterGameplayEffect::StaticClass()->GetDefaultObject()), character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
	character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(spec);
}

void UWaterBlockTrigger::OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	FGameplayEffectQuery query;
	query.EffectDefinition = UWaterGameplayEffect::StaticClass();
	character->GetAbilitySystemComponent()->RemoveActiveEffects(query);
}

bool UWaterBlockTrigger::IsTrigger(const FullBlock& block, ABaseCharacter* const character) const
{
	return BlockGraphicsHelper::getBlock(character->GetWorld(), conversion::ueToBlock(character->GetActorLocation()), block.getBlock().getId()).getMaterialType() == EMaterialTypeEnum::Water;
}
