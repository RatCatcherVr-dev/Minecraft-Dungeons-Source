#include "Dungeons.h"
#include "game/merchant/currency/EmeraldsCurrency.h"
#include "game/merchant/slot/BuyableItemSlot.h"
#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/quest/ObjectiveTagQuest.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/quest/ProgressStatQuest.h"
#include "game/merchant/transaction/RestockSlots.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/ItemRarityChance.h"
#include "game/merchant/restock/MerchantPredicateRarityGenerator.h"
#include "game/merchant/quest/RestockCountQuest.h"
#include "game/merchant/restock/MerchantRestockItemSettings.h"
#include "VillageMerchant.h"

const std::string AVillageMerchant::RescueVillagerObjectiveTag = "Objective_RescuedVillager";
const FText AVillageMerchant::RescueVillagerQuestText = NSLOCTEXT("Merchant", "RescueVillagersQuest", "Villagers Rescued");
const FText AVillageMerchant::RescueVillagerQuestExplainerText = NSLOCTEXT("Merchant", "RescueVillagersQuest_explainer", "Rescue captured Villagers during missions.");
AVillageMerchant::AVillageMerchant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "VillageMerchant_desc", "This merchant sells common and rare items at a fair price.");
	mLockedSlotsHint = NSLOCTEXT("Merchant", "VillageMerchant_lockedhint", "More items are available as you rescue more Villagers.");
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mAlwaysSelectSelectionClass = USelectMerchantSlot::StaticClass();
	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()} };
	mPermanentTransactionClasses = {URestockSlots::StaticClass()};

	mMerchantCurrencyComponent = CreateDefaultSubobject<UEmeraldsCurrency>(TEXT("Currency"));

	mRestockQuest = UProgressStatQuest::CreateSubobject(this, TEXT("RestockQuest"), EProgressStat::WIN_MISSIONS, 1);

	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 100)
		->SetRestockPricingQuest(50, URestockCountQuest::CreateSubobject(this, TEXT("PricingQuest")));	

	const TArray<game::merchant::restock::FBargainChance> bargains = {
		{0.0f, 5},
		{0.15f, 2},
		{0.3f, 1},
		{0.5f, 1}
	};

	auto itemGenerator = UMerchantPredicateRarityGenerator::CreateSubobject(this, TEXT("ItemGenerator"), {
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::GearVendor(), EItemRarityChanceCategory::VillageMerchant)
				.weight(3.f)
				.priceBargains(bargains)
				.pricePowerRollLuckMultiplier(0.25f)
				.priceMultiplier(1.0f),
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::ItemVendor(), EItemRarityChanceCategory::VillageMerchant)
				.weight(1.f)
				.priceBargains(bargains)
				.pricePowerRollLuckMultiplier(0.25f)
				.priceMultiplier(1.0f),
		});

	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot1"))
		->SetItemGenerator(itemGenerator);
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot2"))
		->SetItemGenerator(itemGenerator);
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot3"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UObjectiveTagQuest::CreateSubobject(this, TEXT("Quest1"), RescueVillagerObjectiveTag, RescueVillagerQuestText, RescueVillagerQuestExplainerText, 5));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot4"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UObjectiveTagQuest::CreateSubobject(this, TEXT("Quest2"), RescueVillagerObjectiveTag, RescueVillagerQuestText, RescueVillagerQuestExplainerText, 15));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot5"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UObjectiveTagQuest::CreateSubobject(this, TEXT("Quest3"), RescueVillagerObjectiveTag, RescueVillagerQuestText, RescueVillagerQuestExplainerText, 30));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot6"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UObjectiveTagQuest::CreateSubobject(this, TEXT("Quest4"), RescueVillagerObjectiveTag, RescueVillagerQuestText, RescueVillagerQuestExplainerText, 70));
}
