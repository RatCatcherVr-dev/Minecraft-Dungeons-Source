#include "Dungeons.h"
#include "game/merchant/currency/EmeraldsCurrency.h"
#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/slot/GiftWrappingItemSlot.h"
#include "game/merchant/quest/ProgressStatQuest.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/selection/SelectOtherPlayerCharacter.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/ItemRarityChance.h"
#include "game/merchant/restock/MerchantPredicateRarityGenerator.h"
#include "GiftWrapperMerchant.h"

AGiftWrapperMerchant::AGiftWrapperMerchant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "GiftWrapperMerchant_desc", "Gifts items to other players in your camp.");
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "GiftWrapperMerchant_bullet1", "Will reroll item power to match the receiving player's recommended difficulty"), EBulletIcon::Generic });
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "GiftWrapperMerchant_bullet2", "Instantly returns any invested enchantment points"), EBulletIcon::Generic });
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mAlwaysSelectSelectionClass = USelectMerchantSlot::StaticClass();
	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()}, {USelectOtherPlayerCharacter::StaticClass()}, {USelectInventorySlotItem::StaticClass()} };
	mMerchantCurrencyComponent = CreateDefaultSubobject<UEmeraldsCurrency>(TEXT("Currency"));
	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 100);
	mRestockQuest = UProgressStatQuest::CreateSubobject(this, TEXT("RestockQuest"), EProgressStat::WIN_MISSIONS, 1);

	auto itemGenerator = UMerchantPredicateRarityGenerator::CreateSubobject(this, TEXT("ItemGenerator"), {		
		{ game::item::generator::predicates::Only(game::item::type::GiftBox.getId()), EItemRarityChanceCategory::OnlyCommon },
		});

	CreateDefaultSlotSubobject<UGiftWrappingItemSlot>(TEXT("Slot1"))
		->SetItemGenerator(itemGenerator);	
}