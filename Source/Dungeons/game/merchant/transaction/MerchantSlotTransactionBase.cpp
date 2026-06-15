#include "Dungeons.h"
#include "MerchantSlotTransactionBase.h"

void UMerchantSlotTransactionBase::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	Super::PopulateRequiredSelectionClasses(selections);
	selections.Add(USelectMerchantSlot::StaticClass());
}
