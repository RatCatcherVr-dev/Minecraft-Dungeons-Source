#include "Dungeons.h"
#include "EnchantmentGeneratorConfigs.h"
#include "game/difficulty/Difficulty.h"
#include "game/item/power/ItemPowerStats.h"
#include "util/FloatRange.h"
#include "util/Math.h"

namespace game { namespace enchantment { namespace generator { namespace configs {

float bossBonus(bool isBoss) {
	// boss should increase 0.4 in mission "difficulty" / 5 levels to get unit range (assuming uniformity etc)
	return isBoss ? (0.4f / 5) : 0;
}

std::function<float()> createRandom(bool isBoss, bool isUnique) {
	if (isBoss && isUnique) {
		return [] { return FloatRange(0.7f, 1).random(); };
	}
	if (isBoss || isUnique) {
		return [] { return FloatRange(0.5f, 1).random(); };
	}
	return [] { return FloatRange(0, 1).random(); };
}

Counts FromDifficultyFraction(float fraction, bool isBoss, bool isUnique) {
	const auto fractionWithBoss = fraction + bossBonus(isBoss);
	const auto clampedFraction = Math::clamp(fractionWithBoss, 0.0f, 1.0f);
	const auto rnd = createRandom(isBoss, isUnique);
	const auto rows = Math::clamp(Math::round(0.5f + fractionWithBoss * 1.75f + std::pow(rnd(), 1.5f)), 1, 3);
	const auto minCountPerRow = 2;
	const auto count = [&] { 
		const auto res = rows * (1 + 2 * std::pow(rnd(), 2.0f / (1 + clampedFraction)));
		return Math::clamp(Math::round(res), rows * minCountPerRow, 9);
	}();

	auto powerfulRows = 0;
	if (clampedFraction >= 0.8f) {
		powerfulRows = 3;
	}
	else if (clampedFraction >= 0.56) {
		powerfulRows = 2;
	}
	else if (clampedFraction >= 0.36f) {
		powerfulRows = 1;
	}
	const auto powerfulCount = Math::round(std::pow(rnd(), 0.5f) + 5 * clampedFraction * std::pow(rnd(), 1.5f));
	return { rows, minCountPerRow, count, powerfulRows, powerfulCount };
}

Counts NetheriteFromDifficultyFraction(float fraction, bool isBoss /*= false*/, bool isUnique /*= false*/) {	
	const auto v = Math::clamp(fraction + bossBonus(isBoss), 0.0f, 1.0f);
	const auto rndRes = createRandom(isBoss, isUnique)();
	const auto minCountPerRow = 1;
	const auto count = 1;
	const auto powerfulCount = ((rndRes + v) >= 1.9f) ? 1 : 0;

	return { count, minCountPerRow, count, powerfulCount, powerfulCount };
}



Counts FromDifficulty(const FDifficulty& difficulty, bool isBoss, bool isUnique) {
	return FromDifficultyFraction(difficulty.combinedFractionIncludingEndlessStruggle(), isBoss, isUnique);
}

Counts NetheriteFromDifficulty(const FDifficulty& difficulty, bool isBoss /*= false*/, bool isUnique /*= false*/) {
	return NetheriteFromDifficultyFraction(difficulty.combinedFractionIncludingEndlessStruggle(), isBoss, isUnique);
}

Counts ReEnchantFromDifficulty(const FDifficulty& difficulty, const bool isUnique) {
	return NetheriteFromDifficultyFraction(difficulty.combinedFractionIncludingEndlessStruggle(), false, isUnique);
}

Counts FromItemPower(float itemPower, bool isBoss, bool isUnique) {
	return FromDifficultyFraction(game::item::power::GetItemPowerProgressFraction(itemPower), isBoss, isUnique);
}

}}}}
