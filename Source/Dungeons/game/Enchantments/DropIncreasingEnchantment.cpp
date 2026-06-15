#include "Dungeons.h"
#include "DropIncreasingEnchantment.h"
#include "AbilitySystemComponent.h"
#include "game/util/ValueFormat.h"
#include "game/actor/character/player/PlayerCharacter.h"


UDropIncreasingEnchantment::UDropIncreasingEnchantment() {
	LevelMultiplier = [this](int level) -> float {
		return (DropRateIncreaseBaseChance + level * DropRateIncreasePerLevel);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UDropIncreasingEnchantment::OnStart() {
	SetIsCurrentlyRolling(false);
	Super::OnStart();
}

bool UDropIncreasingEnchantment::OnAfterDropRolled(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, const float probability) {
	if (IsDropQualified(dropData, dropSource)) {
		SetIsCurrentlyRolling(true);
		if (IsSuccessfulReroll(probability)) {
			BroadcastEnchantmentTriggeredEvent();
			ExecuteGamePlayCue(dropSource.DropLocation);
			return true;
		}
	}
	return false;
}

void UDropIncreasingEnchantment::ExecuteGamePlayCue(const FVector location) {
	const auto characterOwner = GetCharacterOwner();
	auto* abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayCueParameters params;
	params.Location = location;
	params.Instigator = characterOwner;
	AddGameplayCueParameters(params);
	abilitySystem->ExecuteGameplayCue(DropGameplayCue, params);
}

void UDropIncreasingEnchantment::AddGameplayCueParameters(FGameplayCueParameters& params) {	
}

/**
 * Criteria:
 * - Enchantment isn't already rolling.
 * - The drop is of the correct category for this enchantment.
 * - The damage type is correct for this enchantment.
 */
bool UDropIncreasingEnchantment::IsDropQualified(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource) const {
	return !IsCurrentlyRolling() && dropData.Category == DropCategory;
}

bool UDropIncreasingEnchantment::IsSuccessfulReroll(const float probability) const {
	return FMath::FRand() < GetRerollProbability(probability);
}

float UDropIncreasingEnchantment::GetRerollProbability(const float probability) const {
	const auto modifiedProbability = FMath::Clamp(bAlwaysTrigger ? 1 : probability * LevelMultiplier(Level), 0.f, 1.f);
	return 1.f - ((1.f - modifiedProbability) / (1.f - probability));
}
