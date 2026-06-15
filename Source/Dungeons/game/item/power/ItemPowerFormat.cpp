#include "Dungeons.h"
#include "ItemPowerFormat.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/util/ValueFormat.h"

#define LOCTEXT_NAMESPACE "ItemPowerFormat"

namespace game { namespace item { namespace power { namespace format {



DUNGEONS_API extern FString AbsoluteDamage(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier)
{		
	const float lowest = instance->GetStats(EItemStats::LowestDamage);
	const float highest = instance->GetStats(EItemStats::HighestDamage);
	const float projectiles = instance->GetStats(EItemStats::ProjectilesPerAttack);	
	FText DamageText = FText::FromString(valueformat::asDamage(lowest*multiplier));
	if (lowest != highest) {
		DamageText = FText::Format(LOCTEXT("DamageRangeTemplate", "{0}-{1}"), DamageText, FText::FromString(valueformat::asDamage(highest*multiplier)));
	}
	if (projectiles > 1.0f) {
		DamageText = FText::Format(LOCTEXT("DamageManyProjectilesTemplate", "{0} x {1}"), DamageText, FText::FromString(valueformat::asConstant(projectiles)));
	}

	return DamageText.ToString();	
}

DUNGEONS_API extern FString AbsoluteDamagePerSecond(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier)
{
	const float dps = instance->GetStats(EItemStats::DamagePerSecond);	
	return valueformat::asDamage(dps*multiplier);
}

DUNGEONS_API extern FString AbsoluteMaxHealthIncrease(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier) {

	float defaultHealth = 500.0f;
	if (auto hc = owner.FindComponentByClass<UHealthComponent>()) {
		defaultHealth = hc->MaxHealth;
	}
	const float totalHealthIncrease = (defaultHealth * multiplier) - defaultHealth;
	return valueformat::asRelativeConstant(valueformat::healthAsDisplayAmount(totalHealthIncrease));
}

DUNGEONS_API extern FString AbsoluteItemHealth(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier)
{
	const float totalHealth = instance->GetStats(EItemStats::Health);
	return valueformat::asDamage(totalHealth*multiplier);
}

DUNGEONS_API extern FString AbsoluteHealthHealed(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier)
{
	const float lowest = instance->GetStats(EItemStats::LowestHealthHealed);
	const float highest = instance->GetStats(EItemStats::HighestHealthHealed);
	FText HealthHealedText = FText::FromString(valueformat::asDamage(lowest*multiplier));
	if (lowest != highest) {
		HealthHealedText = FText::Format(LOCTEXT("HealthHealedRangeTemplate", "{0}-{1}"), HealthHealedText, FText::FromString(valueformat::asDamage(highest*multiplier)));
	}	
	return HealthHealedText.ToString();
}

}}}}


#undef LOCTEXT_NAMESPACE
