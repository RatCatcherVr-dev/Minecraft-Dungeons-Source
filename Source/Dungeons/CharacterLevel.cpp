#include "Dungeons.h"
#include "CharacterLevel.h"

TableCharacterLevels::TableCharacterLevels(std::vector<int> limits, int asymptoticalDifference)
	: mAsymptoticalDifference(asymptoticalDifference)
	, mCutoffLimit(limits.back() + mAsymptoticalDifference)
{
	add(0);
	for (auto limit : limits) {
		add(limit);
	}
	for (size_t i = 0; i < mLevels.size() - 1; ++i) {
		mLevels[i].nextLevelXp = mLevels[i + 1].lastLevelXp;
	}
	mLevels.back().nextLevelXp = mCutoffLimit;
}

FCharacterLevel TableCharacterLevels::createCharacterLevel(int xp) const {
	if (xp >= mCutoffLimit) {
		const int levelsAbove = 1 + (xp - mCutoffLimit) / mAsymptoticalDifference;
		const int levelXp = mLevels.back().lastLevelXp + levelsAbove * mAsymptoticalDifference;
		return{ mLevels.back().level + levelsAbove, xp, levelXp, levelXp + mAsymptoticalDifference };
	}
	auto level = *std::lower_bound(mLevels.begin(), mLevels.end(), xp, [](const FCharacterLevel& it, int val) {
		return val >= it.nextLevelXp;
	});
	level.currentXp = xp;
	return level;
}

FCharacterLevel TableCharacterLevels::createCharacterLevelForLevel(int level) const {
	const int clampedLevel = FMath::Max(0, level - 1); // -1 because incoming 'level' is 1-based
	const int levelsAbove = clampedLevel - mLevels.back().level;
	const int xp = levelsAbove >= 0 ? mCutoffLimit + levelsAbove * mAsymptoticalDifference : mLevels[clampedLevel].lastLevelXp;
	return createCharacterLevel(xp);
}

void TableCharacterLevels::add(int xp) {
	mLevels.push_back({ (int)mLevels.size() + 1, 0, xp, 0 });
}

//
// Factory
//
TableCharacterLevels defaultIncreasingCharacterLevels(int initialIncrement, int incrementIncrease, int upUntilIncrementIs) {
	auto generator = [xp = 0, inc = initialIncrement - incrementIncrease, incrementIncrease](size_t) mutable {
		inc += incrementIncrease;
		return xp += inc;
	};
	return increasingCharacterLevels(generator, upUntilIncrementIs);
}
