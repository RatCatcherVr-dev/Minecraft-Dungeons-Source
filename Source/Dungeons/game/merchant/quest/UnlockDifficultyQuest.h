#pragma once
#include "MerchantQuestBase.h"
#include "UnlockDifficultyQuest.generated.h"

class AMerchantBase;

UCLASS()
class DUNGEONS_API UUnlockDifficultyQuest : public UMerchantQuestBase {
	GENERATED_BODY()
	EGameDifficulty difficultyRequired = EGameDifficulty::Difficulty_1;
public:
	static UUnlockDifficultyQuest* CreateSubobject(AMerchantBase* merchant, const FName& name, EGameDifficulty required);
public:
	bool IsCompleted() const override;
	float GetProgressFraction() const override;	
	int GetRequiredMissionsCount() const;
	int GetCompletedRequiredMissionsCount() const;
	FText GetProgressText() const override;
	FText GetExplainerText() const override;
	FText GetCountText() const override;
};