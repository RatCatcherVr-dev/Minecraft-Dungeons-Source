#pragma once
#include "game/item/generator/ItemGeneratorTypes.h"
#include "game/item/ItemRarityChance.h"
#include "MerchantItemGeneratorBase.h"
#include "MerchantRestockItemSettings.h"
#include "MerchantPredicateRarityGenerator.generated.h"

UCLASS()
class DUNGEONS_API UMerchantPredicateRarityGenerator : public UMerchantItemGeneratorBase {
	GENERATED_BODY()
private:
	TArray<game::merchant::restock::ItemSettings> mWeightedDropSettings;
public:
	static UMerchantPredicateRarityGenerator* CreateSubobject(AMerchantBase* merchant, const FName& name, const game::item::generator::Pred&, EItemRarityChanceCategory);
	static UMerchantPredicateRarityGenerator* CreateSubobject(AMerchantBase* merchant, const FName& name, const TArray<game::merchant::restock::ItemSettings>&);

	TOptional<FMerchantItemData> generate() const;
};