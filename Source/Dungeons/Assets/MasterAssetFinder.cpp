#include "MasterAssetFinder.h"
#include "ItemAssetFinder.h"
#include "EnchantmentAssetFinder.h"
#include "ArmorPropertyAssetFinder.h"
#include "AmbienceFinder.h"

UMasterAssetFinder::UMasterAssetFinder() {
	ItemAssetFinder = CreateDefaultSubobject<UItemAssetFinder>(TEXT("ItemAssetFinder"));
	EnchantmentAssetFinder = CreateDefaultSubobject<UEnchantmentAssetFinder>(TEXT("EnchantmentAssetFinder"));
	ArmorPropertyAssetFinder = CreateDefaultSubobject<UArmorPropertyAssetFinder>(TEXT("ArmorPropertyAssetFinder"));
	AmbienceFinder =  CreateDefaultSubobject<UAmbienceFinder>(TEXT("AmbienceFinder"));
}

void UMasterAssetFinder::Initialize(bool force) {
	ItemAssetFinder->Initialize(force);
	EnchantmentAssetFinder->Initialize(force);
	ArmorPropertyAssetFinder->Initialize(force);
	AmbienceFinder->Initialize(force);
}
