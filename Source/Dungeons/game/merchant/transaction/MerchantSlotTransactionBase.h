#pragma once
#include "MerchantTransactionBase.h"
#include "game/merchant/slot/MerchantSlotBase.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "MerchantSlotTransactionBase.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UMerchantSlotTransactionBase : public UMerchantTransactionBase {
	GENERATED_BODY()
protected:
	void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const override;
public:
	
	template <class T>
	T* GetSelectedSlot() const {
		if (auto selection = GetContext().GetMerchant().GetSelection<USelectMerchantSlot>()) {
			return Cast<T>(selection->GetMerchantSlot());
		}
		return nullptr;
	}
};