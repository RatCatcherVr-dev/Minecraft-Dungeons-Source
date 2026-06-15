#include "Dungeons.h"
#include "PiglinMerchant.h"
#include "game/merchant/transaction/RestockSlots.h"
#include "game/merchant/slot/BuyableItemSlot.h"
#include "game/merchant/quest/ProgressStatQuest.h"
#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/currency/GoldCurrency.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/restock/MerchantPredicateRarityGenerator.h"
#include "game/merchant/quest/RestockCountQuest.h"
#include "game/merchant/restock/MerchantRestockItemSettings.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/ItemRarityChance.h"
#include "game/util/ValueFormat.h"
#include "game/mob/MobTypeDefs.h"
#include "world/entity/MobTags.h"
#include "util/Algo.h"

APiglinMerchant::APiglinMerchant(const FObjectInitializer& ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "PiglinMerchant_desc", "This merchant only trades for Gold and carries exclusive Gilded items.");
	mLockedSlotsHint = NSLOCTEXT("Merchant", "PiglinMerchant_lockedhint", "Will offer more items if you complete Ancient Hunts.");
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mAlwaysSelectSelectionClass = USelectMerchantSlot::StaticClass();
	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()} };
	mPermanentTransactionClasses = { URestockSlots::StaticClass() };

	mMerchantCurrencyComponent = CreateDefaultSubobject<UGoldCurrency>(TEXT("Currency"));

	mRestockQuest = UProgressStatQuest::CreateSubobject(this, TEXT("RestockQuest"), EProgressStat::WIN_HYPERMISSIONS, 1);

	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 12)
		->SetRestockPricingQuest(5, URestockCountQuest::CreateSubobject(this, TEXT("PricingQuest")));

	const TArray<game::merchant::restock::FBargainChance> bargains = {
		{0.0f, 12},
		{0.10f, 4},
		{0.20f, 3},
		{0.30f, 2}
	};

	auto itemGenerator = UMerchantPredicateRarityGenerator::CreateSubobject(this, TEXT("ItemGenerator"), {
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::GearVendor(), EItemRarityChanceCategory::PiglinMerchant)
				.weight(1.f)
				.itemSource(itemgen::ItemSource::NetheriteMerchant)
				.priceBargains(bargains)
				.pricePowerRollLuckMultiplier(1.0f)
				.priceMultiplier(1.0f)
		});

	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot1"))
		->SetItemGenerator(itemGenerator);
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot2"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest1"), EProgressStat::WIN_HYPERMISSIONS, 3));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot3"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest2"), EProgressStat::WIN_HYPERMISSIONS, 15));
}
