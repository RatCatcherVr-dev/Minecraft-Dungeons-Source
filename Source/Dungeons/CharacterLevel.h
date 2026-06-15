#pragma once

#include "util/Algo.h"
#include "CharacterLevel.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FCharacterLevel {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 level;

	UPROPERTY(BlueprintReadOnly)
	int32 currentXp;

	UPROPERTY(BlueprintReadOnly)
	int32 lastLevelXp;

	UPROPERTY(BlueprintReadOnly)
	int32 nextLevelXp;
	
	float getProgress() const { return (currentXp - lastLevelXp) / static_cast<float>(nextLevelXp - lastLevelXp); }
};

UCLASS()
class DUNGEONS_API UCharacterLevelUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void getProgress(UPARAM(ref) FCharacterLevel& CharacterLevel, float& LevelProgress) { LevelProgress = CharacterLevel.getProgress(); }
};

class TableCharacterLevels {
public:
	TableCharacterLevels(std::vector<int> limits, int asymptoticalDifference);

	FCharacterLevel createCharacterLevel(int xp) const;
	FCharacterLevel createCharacterLevelForLevel(int level) const;
private:
	void add(int xp);
	int mAsymptoticalDifference;
	int mCutoffLimit;
	std::vector<FCharacterLevel> mLevels;
};

template <typename LimitGenerator>
TableCharacterLevels increasingCharacterLevels(LimitGenerator limit, int upUntil, int asymptoticalDifference = -1) {
	const auto limits = algo::generate::generate_while(limit, algo::generate::predicates::differenceLessThanOrEquals(upUntil));
	return TableCharacterLevels(limits, asymptoticalDifference != -1 ? asymptoticalDifference : upUntil);
}

TableCharacterLevels defaultIncreasingCharacterLevels(int initialIncrement, int incrementIncrease, int upUntilIncrementIs);
