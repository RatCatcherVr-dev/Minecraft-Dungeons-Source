#include "Dungeons.h"
#include "EnchantmentMerchant.h"

#include "game/merchant/pricing/ProgressPowerRarityPricing.h"
#include "game/merchant/currency/GoldCurrency.h"
#include "game/merchant/selection/SelectInventorySlotItemEnchantable.h"
#include "game/merchant/transaction/ModifyItem.h"
#include "locale/LocTableFromFile.h"

AEnchantmentMerchant::AEnchantmentMerchant(const FObjectInitializer&) {
	mDisplayDescription = LocTableFromFile::Get("DLC6MerchantLabels.csv", "merchant_enchantment_description");
	mDisplayBulletPoints.Add({ LocTableFromFile::Get("DLC6MerchantLabels.csv", "merchant_enchantment_bullet1"), EBulletIcon::EnchantmentPoint });
	mAlwaysSelectSelectionClass = USelectInventorySlotItemEnchantable::StaticClass();
	mSelectionClassFocusOrder = { {USelectInventorySlotItemEnchantable::StaticClass()} };

	mMerchantCurrencyComponent = CreateDefaultSubobject<UGoldCurrency>(TEXT("Currency"));
	mMerchantPricingComponent = UProgressPowerRarityPricing::CreateSubobject(this, TEXT("Pricing"), 100);

	mPermanentTransactionClasses = { UModifyItem::StaticClass() };
}
