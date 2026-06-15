#include "Dungeons.h"
#include "game/merchant/selection/SelectAdventureHubSlot.h"
#include "game/merchant/transaction/ClaimAdventureHubReward.h"
#include "AdventureHubMerchant.h"

void AAdventureHubMerchant::OnPlayerInteraction(APlayerCharacter* player) {
	Super::OnPlayerInteraction(player);
}

AAdventureHubMerchant::AAdventureHubMerchant(const FObjectInitializer& ObjectInitializer) {
	mDisplayDescription = NSLOCTEXT("Merchant", "AdventureHub_desc", "");

	mPermanentSelectionClass = USelectAdventureHubSlot::StaticClass();
	mPermanentTransactionClasses = { UClaimAdventureHubReward::StaticClass() };
	mSelectionClassFocusOrder = { { USelectAdventureHubSlot::StaticClass() } };
}
