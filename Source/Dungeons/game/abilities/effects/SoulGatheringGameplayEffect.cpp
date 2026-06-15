#include "SoulGatheringGameplayEffect.h"
#include "game/abilities/attributes/ItemAttributeSet.h"

const FName USoulGatheringGameplayEffect::SoulGatheringMagnitude(TEXT("SoulGathering"));

USoulGatheringGameplayEffect::USoulGatheringGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	FSetByCallerFloat soulGathering;
	soulGathering.DataName = SoulGatheringMagnitude;

	FGameplayModifierInfo soulInfo;
	soulInfo.Attribute = UItemAttributeSet::SoulGatheringAttribute();
	soulInfo.ModifierMagnitude = soulGathering;
	soulInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(soulInfo);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Soul.Gathering")));
}