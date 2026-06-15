#pragma once
#include "MerchantSlotBase.h"
#include "game/merchant/selection/MerchantSelectionBase.h"
#include "MerchantSelectionSlotBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMerchantSelectionSlotChanged, const UMerchantSelectionBase*, selection);

UCLASS(BlueprintType)
class DUNGEONS_API UMerchantSelectionSlotBase : public UMerchantSlotBase {
	GENERATED_BODY()
private:

	UPROPERTY()
	TSubclassOf<UMerchantSelectionBase> mSelectionClass;

	void OnSelectionChanged(const UMerchantSelectionBase&) const;

protected:
	void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const override;

public:
	template <class T>
	static T* CreateSubobject(UObject* object, const FName& name, const TSubclassOf<UMerchantSelectionBase>& selectionClass) {		
		checkf(selectionClass.Get(), TEXT("invalid selection class"));
		auto slot = object->CreateDefaultSubobject<T>(name);
		slot->mSelectionClass = selectionClass;
		if (auto merchant = slot->GetMerchantMutable()) {
			if (auto selection = merchant->GetOrCreateSelectionByClass(selectionClass)) {
				selection->OnSelectionChangedInternal.AddUObject(slot, &T::OnSelectionChanged);
			}
		}		
		return slot;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TSubclassOf<UMerchantSelectionBase>& GetSelectionClass() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantSelectionBase* GetSelection() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantSelectionSlotChanged OnMerchantSelectionSlotChanged;
};
