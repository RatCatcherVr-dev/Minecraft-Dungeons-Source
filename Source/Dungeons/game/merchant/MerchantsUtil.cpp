#include "Dungeons.h"
#include "game/merchant/MerchantsUtil.h"
#include "MerchantDefs.h"
#include "util/Algo.hpp"
#include "game/util/UnlockKeyUtils.h"
#include "game/util/DungeonsTravelUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"

int UMerchantsUtil::GetUnlockedMerchantsCount(UObject* worldContext) {
	return algo::count_if(merchantdefs::getAllEnabledClasses(), [worldContext](const TSubclassOf<UMerchantDef>& def) {
		return UUnlockKeyUtils::IsUnlocked(worldContext->GetWorld(), Cast<UMerchantDef>(def->GetDefaultObject())->GetUnlockProgressKey());
	});
}

int UMerchantsUtil::GetTotalMerchantsCount() {
	return merchantdefs::getAllEnabledClasses().Num();
}

bool UMerchantsUtil::IsMerchantLockedForLevel(UObject* worldContext, ELevelNames level) {
	TArray<TSubclassOf<UMerchantDef>> defs;
	defs = algo::copy_if(merchantdefs::getAllEnabledClasses(), [level](const TSubclassOf<UMerchantDef>& def) { return Cast<UMerchantDef>(def->GetDefaultObject())->GetLegacyUnlockMission() == level; });
	return algo::any_of(defs, [worldContext](const TSubclassOf<UMerchantDef>& def) {
		return !UUnlockKeyUtils::IsUnlocked(worldContext->GetWorld(), Cast<UMerchantDef>(def->GetDefaultObject())->GetUnlockProgressKey()); 
	});
}

bool UMerchantsUtil::IsMerchantUnlocked(UObject* WorldContextObject, TSubclassOf<UMerchantDef> def) {
	if(def){
		return UUnlockKeyUtils::IsUnlocked(WorldContextObject->GetWorld(), Cast<UMerchantDef>(def->GetDefaultObject())->GetUnlockProgressKey());
	}
	return false;
}

void UMerchantsUtil::UnlockMerchantForAllLocalPlayers(UObject* WorldContextObject, TSubclassOf<UMerchantDef> def) {
	check(def && "trying to unlock a nullptr merchant class");
	if(def){
		for (auto* player : InstanceTracker<APlayerCharacter>::GetList(WorldContextObject->GetWorld())) {
			if (player->IsLocallyControlled()) {
				player->ClientUnlockMerchant(def);
			}
		}	
	}
}

const FText& UMerchantsUtil::GetMerchantName(TSubclassOf<UMerchantDef> def) {
	check(def && "trying to get from a nullptr merchant class");
	if (def) {
		return Cast<UMerchantDef>(def->GetDefaultObject())->GetDisplayName();
	}
	return FText::GetEmpty();
}
