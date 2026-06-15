#include "Dungeons.h"
#include "game/levels.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/ShopperComponent.h"
#include "game/merchant/slot/OfferedItemSlot.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/selection/SelectInventorySlotItemMelee.h"
#include "game/merchant/selection/SelectInventorySlotItemArmor.h"
#include "game/merchant/selection/SelectInventorySlotItemRanged.h"
#include "game/merchant/selection/SelectInventorySlotItemArtifact.h"
#include "game/merchant/selection/OfferEnchantmentPoints.h"
#include "game/merchant/transaction/OfferHyperMissionOfferings.h"
#include "game/merchant/transaction/MissionOfferingsTransactionBase.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "game/inventory/InventoryItemUtil.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "util/Algo.h"
#include "HyperMissionMerchant.h"

namespace hypermission {
	int getItemOfferedEnchantmentPointsMaxIncrease(const FInventoryItemData& item) {
		return item.GetNumEnchantmentSlots();			
	}

	ELevelNames mission = ELevelNames::netherhypermission;
}

void AHyperMissionMerchant::RefreshOfferings() {
	if (auto offering = GetTransaction<UOfferHyperMissionOfferings>()) {
		if (!offering->TryExecute()) {
			if (auto itemStash = GetCurrentSession().GetItemStashComponentMutable()) {
				itemStash->ClearMissionOfferings(offering->GetMission());
			}
		}
	}
}

void AHyperMissionMerchant::OnPlayerInteraction(APlayerCharacter* player) {
	if (auto controller = player->GetController()) {
		if (auto shopper = controller->FindComponentByClass<UShopperComponent>()) {
			if (auto progress = player->FindComponentByClass<UMissionProgressComponent>()) {
				if (progress->GetMissionCanContinue(hypermission::mission) || 
					!progress->IsMissionUnlocked(EGameDifficulty::Difficulty_1, hypermission::mission)) {					
					shopper->OpenMissionSelector(hypermission::mission);
					return;
				}
			}
		}
	}
	Super::OnPlayerInteraction(player);
}

AHyperMissionMerchant::AHyperMissionMerchant(const FObjectInitializer& ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "HyperMission_desc", "You must offer at least one item to begin an Ancient Hunt.");
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "HyperMission_bullet1", "Offering Enchantment Points will attract more Ancient mobs, and the Enchantment Points will be spent once you start the mission."), EBulletIcon::EnchantmentPoint });
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "HyperMission_bullet3", "Each spent Enchantment Point will reduce your Hero Level by one."), EBulletIcon::EnchantmentPoint });
	mDisplayBulletPoints.Add({ NSLOCTEXT("Merchant", "HyperMission_bullet2", "The combination of runes on your offered items will affect which Ancient mobs you may encounter."), EBulletIcon::Archetype });

	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
	mPermanentTransactionClasses = { UOfferHyperMissionOfferings::StaticClass() };

	TSubclassOf<UMerchantSelectionBase> fallbackClass = USelectMerchantSlot::StaticClass();

	mSelectionClassFocusOrder = { {USelectMerchantSlot::StaticClass()}, {[fallbackClass](const UMerchantSubobjectBase* obj) -> const TSubclassOf<UMerchantSelectionBase>& {
		if (auto slotSelection = obj->GetMerchant()->GetSelection<USelectMerchantSlot>()) {
			if (auto slot = slotSelection->GetMerchantSlot()) {
				if (auto selectionSlot = Cast<UOfferedItemSlot>(slot)) {
					return selectionSlot->GetSelectionClass();
				}
			}
		}
		return fallbackClass;
	}} };

	AddSelection(USelectInventorySlotItem::CreateSubobject<USelectInventorySlotItemMelee>(this, TEXT("SelectMelee"), RETLAMBDA(!it->IsNetherite() && !it->IsCloned())));
	AddSelection(USelectInventorySlotItem::CreateSubobject<USelectInventorySlotItemArmor>(this, TEXT("SelectArmor"), RETLAMBDA(!it->IsNetherite() && !it->IsCloned())));
	AddSelection(USelectInventorySlotItem::CreateSubobject<USelectInventorySlotItemRanged>(this, TEXT("SelectRanged"), RETLAMBDA(!it->IsNetherite() && !it->IsCloned())));
	AddSelection(USelectInventorySlotItem::CreateSubobject<USelectInventorySlotItemArtifact>(this, TEXT("SelectArtifact"), RETLAMBDA(!it->IsNetherite() && !it->IsCloned())));

	AddSlot(UOfferedItemSlot::CreateSubobject<UOfferedItemSlot>(this, TEXT("SlotMelee"), USelectInventorySlotItemMelee::StaticClass()));
	AddSlot(UOfferedItemSlot::CreateSubobject<UOfferedItemSlot>(this, TEXT("SlotArmor"), USelectInventorySlotItemArmor::StaticClass()));
	AddSlot(UOfferedItemSlot::CreateSubobject<UOfferedItemSlot>(this, TEXT("SlotRanged"), USelectInventorySlotItemRanged::StaticClass()));
	AddSlot(UOfferedItemSlot::CreateSubobject<UOfferedItemSlot>(this, TEXT("SlotArtifact"), USelectInventorySlotItemArtifact::StaticClass()));

	AddTransaction(UMissionOfferingsTransactionBase::CreateSubobject<UOfferHyperMissionOfferings>(this, TEXT("SetMissionOfferings"), hypermission::mission));
	
	auto* offerPoints = AddSelection(UOfferEnchantmentPoints::CreateSubobject(this, TEXT("OfferEnchantmentPoints"), [this](int32 currentMax) -> int32 {
		const auto maxPossibleDueToEnchantmentPoints = currentMax + algo::sum(GetSlots<UOfferedItemSlot>(), RETLAMBDA(it->GetInventoryItem() ? UInventoryItemUtil::GetTotalInvestedEnchantmentPoints(it->GetInventoryItem()->Item) : 0));
		const auto maxPossibleDueToEnchantmentSlotTypes = algo::sum(GetSlots<UOfferedItemSlot>(), RETLAMBDA(it->GetInventoryItem() ? hypermission::getItemOfferedEnchantmentPointsMaxIncrease(it->GetInventoryItem()->Item) : 0));
		return FMath::Min(maxPossibleDueToEnchantmentPoints, maxPossibleDueToEnchantmentSlotTypes);
	}));


	//Always true to update offerings when enchantment point selection or slots change
	offerPoints->OnSelectionChangedInternal.AddLambda([this](const UMerchantSelectionBase&) {
		RefreshOfferings();
	});

	for (auto* slot : GetSlots<UOfferedItemSlot>()) {
		slot->OnMerchantSlotChangedInternal.AddLambda([this]() {
			RefreshOfferings();
		});
	}
}

