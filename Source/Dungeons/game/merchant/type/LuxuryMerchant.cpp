#include "Dungeons.h"
#include "LuxuryMerchant.h"
#include "game/merchant/transaction/RestockSlots.h"
#include "game/merchant/slot/BuyableItemSlot.h"
#include "game/merchant/quest/ProgressStatQuest.h"
#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/currency/EmeraldsCurrency.h"
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

ALuxuryMerchant::ALuxuryMerchant(const FObjectInitializer& ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "LuxuryMerchant_desc", "This merchant sells high-end items at a not-so-affordable price.");
	mLockedSlotsHint = NSLOCTEXT("Merchant", "LuxuryMerchant_lockedhint", "Will offer more items if you defeat powerful mobs.");
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mAlwaysSelectSelectionClass = USelectMerchantSlot::StaticClass();
	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()} };
	mPermanentTransactionClasses = { URestockSlots::StaticClass() };

	mMerchantCurrencyComponent = CreateDefaultSubobject<UEmeraldsCurrency>(TEXT("Currency"));
	
	mRestockQuest = UProgressStatQuest::CreateSubobject(this, TEXT("RestockQuest"), EProgressStat::WIN_MISSIONS, 1);

	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 200)
		->SetRestockPricingQuest(200, URestockCountQuest::CreateSubobject(this, TEXT("PricingQuest")));

		const TArray<game::merchant::restock::FBargainChance> bargains = {
			{0.0f, 10},
			{0.15f, 2},
			{0.25f, 1},
			{0.4f, 1}
		};

	auto itemGenerator = UMerchantPredicateRarityGenerator::CreateSubobject(this, TEXT("ItemGenerator"), {
			game::merchant::restock::MutableItemSettings( game::item::generator::predicates::GearVendor(), EItemRarityChanceCategory::LuxuryMerchant )
				.weight(30.f)
				.itemSource(itemgen::ItemSource::Merchant)
				.priceBargains(bargains)
				.pricePowerRollLuckMultiplier(0.5f)
				.priceMultiplier(1.0f),
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::ItemVendor(), EItemRarityChanceCategory::LuxuryMerchant )
				.weight(10.f)
				.itemSource(itemgen::ItemSource::Merchant)
				.priceBargains(bargains)
				.pricePowerRollLuckMultiplier(0.5f)
				.priceMultiplier(0.75f),
			game::merchant::restock::MutableItemSettings( game::item::generator::predicates::OneOf({game::item::type::MysteryArmor.getId()}), EItemRarityChanceCategory::LuxuryMerchant )
				.weight(1.f)
				.itemSource(itemgen::ItemSource::Merchant)
				.priceBargains(bargains)
				.pricePowerRollLuckMultiplier(0.5f)
				.priceMultiplier(1.0f),
		});

	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot1"))
		->SetItemGenerator(itemGenerator);
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot2"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest1"), EProgressStat::DEFEAT_EVENT_MOBS, 10));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot3"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest2"), EProgressStat::DEFEAT_EVENT_MOBS, 50));
}
