#include "Dungeons.h"
#include "RewardData.h"
#include "game/item/InventoryItemData.h"

FRewardData::FRewardData()
	: RewardType(ERewardType::LevelCompletion)
{

}

FRewardData::FRewardData(ERewardType rewardType, FInventoryItemData itemData)
	: RewardType(rewardType)
	, ItemData(itemData)
{

}

const FInventoryItemData& URewardDataFunctionLibrary::GetInventoryItemDataFromRewardData(const FRewardData& rewardData)
{
	return rewardData.ItemData;
}

int rewardquery::GetMaxEmergentDifficultyCollectedTokens()
{
	return 6;
}
