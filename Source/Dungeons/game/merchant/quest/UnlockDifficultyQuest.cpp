#include "Dungeons.h"
#include "game/difficulty/DifficultyUtil.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/merchant/type/MerchantBase.h"
#include "UnlockDifficultyQuest.h"
#include "game/mission/MissionDefs.h"
#include "game/util/ValueFormat.h"

UUnlockDifficultyQuest* UUnlockDifficultyQuest::CreateSubobject(AMerchantBase* merchant, const FName& name, EGameDifficulty required) {
	auto quest = merchant->CreateDefaultSubobject<UUnlockDifficultyQuest>(name);
	quest->difficultyRequired = required;
	return quest;
}

bool UUnlockDifficultyQuest::IsCompleted() const {
	auto difficulties = GetContext().GetSerializer().ReadDifficulties();
	return difficulties.unlocked.Get(EGameDifficulty::Invalid) >= difficultyRequired;	
}

float UUnlockDifficultyQuest::GetProgressFraction() const {
	return static_cast<float>(GetCompletedRequiredMissionsCount()) / static_cast<float>(GetRequiredMissionsCount());
}

int UUnlockDifficultyQuest::GetRequiredMissionsCount() const {
	switch (difficultyRequired) {
	case EGameDifficulty::Difficulty_1:
	case EGameDifficulty::Difficulty_2:	
		return UMissionDefs::GetRequiredMissionsTree(ELevelNames::obsidianpinnacle).Num();	
	case EGameDifficulty::Difficulty_3:	
		return 1;
	default:
		return 1;
	}
}

int UUnlockDifficultyQuest::GetCompletedRequiredMissionsCount() const {

	switch (difficultyRequired) {
	case EGameDifficulty::Difficulty_1:
	case EGameDifficulty::Difficulty_2:	
		return GetSession().GetMissionProgress()->GetNumCompletedMissionsInSet(UMissionDefs::GetRequiredMissionsTree(ELevelNames::obsidianpinnacle));
	case EGameDifficulty::Difficulty_3:	
		return GetSession().GetMissionProgress()->GetNumCompletedMissionsInSet({ ELevelNames::obsidianpinnacle }, difficultyquery::getPreviousDifficulty(difficultyRequired).Get(EGameDifficulty::Invalid));	
	default:
		return 0;
	}
}

FText UUnlockDifficultyQuest::GetProgressText() const {	
	return NSLOCTEXT("Merchants", "unlock_quest_completed_required_missions", "Required Missions Completed");	
}

FText UUnlockDifficultyQuest::GetExplainerText() const {
	return UDifficultyUtil::GetUnlockExplainerText(difficultyRequired);
}

FText UUnlockDifficultyQuest::GetCountText() const {
	return valueformat::asCounter(GetCompletedRequiredMissionsCount(), GetRequiredMissionsCount());
}
