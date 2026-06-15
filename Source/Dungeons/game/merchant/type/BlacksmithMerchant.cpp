#include "Dungeons.h"
#include "game/merchant/currency/EmeraldsCurrency.h"
#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/slot/UpgraderItemSlot.h"
#include "game/merchant/quest/UnlockDifficultyQuest.h"
#include "game/merchant/quest/DynamicProgressStatQuest.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/util/ValueFormat.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/difficulty/ThreatLevel.h"
#include "BlacksmithMerchant.h"
#include "game/difficulty/endless/EndlessStruggleTiers.h"

namespace blacksmith {

	FEndlessStruggle endlessStruggleFromStat(EProgressStat stat) {
		return endlesstruggle::tier::getTierFromStat(stat)->unlockedEndlessStruggle();
	}

	struct DifficultyProgressStatTier {
		float ItemPowerThreshold;
		EProgressStat stat;
	};

	DifficultyProgressStatTier getDifficultyProgressStatForApocPlus(EProgressStat stat) {
		return { game::DifficultyStats(game::FDifficulty(EGameDifficulty::Difficulty_3, EThreatLevel::Threat_7, EExtraChallenge::NoExtraChallenge, endlessStruggleFromStat(stat))).GetCombinedItemPowerRange().max(), stat };
	}

	const TArray<DifficultyProgressStatTier> GetItemPowerProgressStatTiers() {
		static const TArray<DifficultyProgressStatTier> statTiers = 
		{   getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER7),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER6),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER5),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER4),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER3),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER2),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER1),
			getDifficultyProgressStatForApocPlus(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER0),
			{ game::DifficultyStats(game::FDifficulty(EGameDifficulty::Difficulty_2, EThreatLevel::Threat_7)).GetCombinedItemPowerRange().max(), EProgressStat::WIN_MISSIONS_APOCALYPSE },
			{ game::DifficultyStats(game::FDifficulty(EGameDifficulty::Difficulty_1, EThreatLevel::Threat_7)).GetCombinedItemPowerRange().max(), EProgressStat::WIN_MISSIONS_ADVENTURE },
		};
		return statTiers;
	};

	EProgressStat GetItemPowerRequiredProgressStat(const float ItemPower) {
		for (auto& tier : GetItemPowerProgressStatTiers()) {
			if (ItemPower > tier.ItemPowerThreshold) {
				return tier.stat;
			}
		}
		return EProgressStat::WIN_MISSIONS_DEFAULT;
	}
}

ABlacksmithMerchant::ABlacksmithMerchant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	const int MissionCount = 3;
	mDisplayDescription = NSLOCTEXT("Merchant", "Blacksmith_desc", "Upgrades items while you complete missions.");
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "Blacksmith_bullet1", "Instantly returns any invested Enchantment Points"), EBulletIcon::Generic });
	mDisplayBulletPoints.Add({ FText::Format(NSLOCTEXT("Merchant", "Blacksmith_bullet2", "To finish an item upgrade, you need to complete {0} missions at a minimum difficulty based on the item's power"), valueformat::getAsWordOrNumber(MissionCount)) , EBulletIcon::Generic });
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "Blacksmith_bullet3", "Item Power is rerolled based on your recommended difficulty" ) , EBulletIcon::Generic });
	mLockedSlotsHint = NSLOCTEXT("Merchant", "Blacksmith_lockedhint", "Will upgrade more items if you unlock higher difficulties.");
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()}, {USelectInventorySlotItem::StaticClass()} };
	mMerchantCurrencyComponent = CreateDefaultSubobject<UEmeraldsCurrency>(TEXT("Currency"));
	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 150);

	
	const auto dynamicQuestStatProvider = [](UUpgraderItemSlot* slot) -> UDynamicProgressStatQuest::ProgressStatProvider {
		return [slot]() -> EProgressStat {
			if (auto item = slot->GetOptionalItem()) {				
				return blacksmith::GetItemPowerRequiredProgressStat(item.GetValue().ItemPower);
			}
			return EProgressStat::WIN_MISSIONS;
		};
	};

	auto slot1 = AddSlot(UUpgraderItemSlot::CreateSubobject(this, TEXT("Slot1")));
	slot1->SetUpgradeQuest(UDynamicProgressStatQuest::CreateSubobject(this, TEXT("UpgradeQuest1"), EProgressStat::WIN_MISSIONS, MissionCount, dynamicQuestStatProvider(slot1)));

	auto slot2 = AddSlot(UUpgraderItemSlot::CreateSubobject(this, TEXT("Slot2")));	
	slot2->SetUpgradeQuest(UDynamicProgressStatQuest::CreateSubobject(this, TEXT("UpgradeQuest2"), EProgressStat::WIN_MISSIONS, MissionCount, dynamicQuestStatProvider(slot2)));
	slot2->SetUnlockQuest(UUnlockDifficultyQuest::CreateSubobject(this, TEXT("Quest1"), EGameDifficulty::Difficulty_2));

	auto slot3 = AddSlot(UUpgraderItemSlot::CreateSubobject(this, TEXT("Slot3")));	
	slot3->SetUpgradeQuest(UDynamicProgressStatQuest::CreateSubobject(this, TEXT("UpgradeQuest3"), EProgressStat::WIN_MISSIONS, MissionCount, dynamicQuestStatProvider(slot3)));
	slot3->SetUnlockQuest(UUnlockDifficultyQuest::CreateSubobject(this, TEXT("Quest2"), EGameDifficulty::Difficulty_3));	
}