#include "Dungeons.h"

#include "MysteryMerchant.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/slot/BuyableItemSlot.h"
#include "game/merchant/currency/EmeraldsCurrency.h"
#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/quest/ProgressStatQuest.h"
#include "game/merchant/transaction/RestockSlots.h"
#include "game/merchant/restock/MerchantPredicateRarityGenerator.h"
#include "game/merchant/quest/RestockCountQuest.h"
#include "game/merchant/restock/MerchantRestockItemSettings.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/ItemRarityChance.h"

AMysteryMerchant::AMysteryMerchant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "MysteryMerchant_desc", "This merchant sells random items from one out of several item categories.");
	mLockedSlotsHint = NSLOCTEXT("Merchant", "MysteryMerchant_lockedhint", "Will offer more items if you defeat enchanted mobs.");
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mAlwaysSelectSelectionClass = USelectMerchantSlot::StaticClass();
	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()} };
	mPermanentTransactionClasses = { URestockSlots::StaticClass() };

	mMerchantCurrencyComponent = CreateDefaultSubobject<UEmeraldsCurrency>(TEXT("Currency"));

	mRestockQuest = UProgressStatQuest::CreateSubobject(this, TEXT("RestockQuest"), EProgressStat::WIN_MISSIONS, 1);

	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 150)
		->SetRestockPricingQuest(50, URestockCountQuest::CreateSubobject(this, TEXT("PricingQuest")));		
	
	auto itemGenerator = UMerchantPredicateRarityGenerator::CreateSubobject(this, TEXT("ItemGenerator"), {
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::Only(game::item::type::MysteryBoxAny.getId()), EItemRarityChanceCategory::OnlyCommon)
				.weight(2.0f)
				.priceMultiplier(1.25f),
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::Only(game::item::type::MysteryBoxArtifact.getId()), EItemRarityChanceCategory::OnlyCommon)
				.weight(2.0f)
				.priceMultiplier(1.5f),
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::Only(game::item::type::MysteryBoxMelee.getId()), EItemRarityChanceCategory::OnlyCommon)
				.weight(1.0f)
				.priceMultiplier(1.0f),
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::Only(game::item::type::MysteryBoxRanged.getId()), EItemRarityChanceCategory::OnlyCommon)
				.weight(1.0f)
				.priceMultiplier(1.0f),
			game::merchant::restock::MutableItemSettings(game::item::generator::predicates::Only(game::item::type::MysteryBoxArmor.getId()), EItemRarityChanceCategory::OnlyCommon)
				.weight(1.0f)
				.priceMultiplier(1.0f),
		});

	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot1"))
		->SetItemGenerator(itemGenerator);
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot2"))
		->SetItemGenerator(itemGenerator);
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot3"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest1"), EProgressStat::DEFEAT_ENCHANTED_MOBS, 5));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot4"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest2"), EProgressStat::DEFEAT_ENCHANTED_MOBS, 25));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot5"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest3"), EProgressStat::DEFEAT_ENCHANTED_MOBS, 250));
	CreateDefaultSlotSubobject<UBuyableItemSlot>(TEXT("Slot6"))
		->SetItemGenerator(itemGenerator)
		->SetUnlockQuest(UProgressStatQuest::CreateSubobject(this, TEXT("Quest4"), EProgressStat::DEFEAT_ENCHANTED_MOBS, 2500));
}