#include "Dungeons.h"
#include "ItemPowerEffect.h"
#include "util/CollectionUtils.h"

float UItemPowerEffect::GetMultiplier(float power) const {
	return ItemPowerFunc(power);
}

FText UItemPowerEffect::CreateText(float power) const {
	const float multiplier = GetMultiplier(power);
	return FText::Format(PowerEffectTemplate, FText::FromString(PowerMultiplierFormatter(multiplier)));
}

FText UItemPowerEffect::CreateAbsoluteText(const ABaseCharacter& owner, const AItemInstance* instance, float power) const
{
	if (PowerMultiplierAbsoluteFormatter) {
		const float multiplier = GetMultiplier(power);
		return FText::Format( ! PowerEffectAbsoluteTemplate.IsEmpty() ? PowerEffectAbsoluteTemplate : PowerEffectTemplate, FText::FromString(PowerMultiplierAbsoluteFormatter(owner, instance, multiplier)));
	}	
	return FText::GetEmpty();
}

bool UItemPowerEffect::IsHiddenFromPlayer() const
{
	return HiddenFromPlayer;
}
