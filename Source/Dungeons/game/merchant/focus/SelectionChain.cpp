#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "SelectionChain.h"
#include "util/Algo.h"


SelectionClassPicker::SelectionClassPicker(const Provider& provider)
	: mProvider(provider) {}
SelectionClassPicker::SelectionClassPicker(TSubclassOf<UMerchantSelectionBase> selectionClass)
	: mProvider([selectionClass](const UMerchantSubobjectBase*) -> TSubclassOf<UMerchantSelectionBase> {return selectionClass; }) {}

TSubclassOf<UMerchantSelectionBase> SelectionClassPicker::getSelectionClass(const UMerchantSubobjectBase* obj) const {
	return mProvider(obj);
}

USelectionChain* USelectionChain::CreateObject(AMerchantBase* merchant, const FName& name, const TArray<SelectionClassPicker>& selectionOrder) {
	auto focus = NewObject<USelectionChain>(merchant, name);
	focus->mFocusedSelectionClassOrder = selectionOrder;
	focus->NavigateRelative(0);
	return focus;
}

const TSubclassOf<UMerchantSelectionBase>& USelectionChain::GetSelectionClass() const {
	return mFocusedSelectionClass;
}

TOptional<int> USelectionChain::FindSelectionClassIndex(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {		
	return algo::index_of_if(mFocusedSelectionClassOrder, RETLAMBDA(it.getSelectionClass(this) == selectionClass));
}

void USelectionChain::NavigateRelative(int direction) {
	int targetindex = 0;
	if (mFocusedSelectionClass) {
		if (auto foundIndex = FindSelectionClassIndex(mFocusedSelectionClass)) {
			targetindex = FMath::Clamp(foundIndex.GetValue() + direction, 0, mFocusedSelectionClassOrder.Num());
		}
	}
	if (mFocusedSelectionClassOrder.IsValidIndex(targetindex)) {
		SetFocusedSelectionClass(mFocusedSelectionClassOrder[targetindex].getSelectionClass(this));
	}
}

TSubclassOf<UMerchantSelectionBase> USelectionChain::GetLastSelectionClass(SelectionClassFilter filter) const {
	TSubclassOf<UMerchantSelectionBase> bestSelectionClass;
	for (auto selectionClassPicker : mFocusedSelectionClassOrder) {
		auto selectionClass = selectionClassPicker.getSelectionClass(this);
		if (filter(selectionClass)) {
			bestSelectionClass = selectionClass;
		}
	}
	return bestSelectionClass;
}

TSubclassOf<UMerchantSelectionBase> USelectionChain::GetFirstSelectionClass(SelectionClassFilter filter) const {
	for (auto selectionClassPicker : mFocusedSelectionClassOrder) {
		auto selectionClass = selectionClassPicker.getSelectionClass(this);
		if(filter(selectionClass)){
			return selectionClass;
		}		
	}
	return {};
}

void USelectionChain::SetFocusedSelectionClass(const TSubclassOf<UMerchantSelectionBase>& focusedSelectionClass) {
	if (mFocusedSelectionClass != focusedSelectionClass) {
		mFocusedSelectionClass = focusedSelectionClass;
		OnFocusedSelectionClassChanged.Broadcast();
		OnFocusedSelectionClassChangedInternal.Broadcast();
	}
}

bool USelectionChain::ContainsSelectionClass(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return algo::any_of(mFocusedSelectionClassOrder, RETLAMBDA(it.getSelectionClass(this) == selectionClass));
}

bool USelectionChain::CanNavigateNext() const {
	return mFocusedSelectionClass ? FindSelectionClassIndex(mFocusedSelectionClass).Get(0) < mFocusedSelectionClassOrder.Num() : true;
}

bool USelectionChain::CanNavigatePrevious() const {
	return mFocusedSelectionClass ? FindSelectionClassIndex(mFocusedSelectionClass).Get(0) > 0 : true;
}

void USelectionChain::NavigatePrevious() {
	NavigateRelative(-1);
}

void USelectionChain::NavigateNext() {
	NavigateRelative(1);
}

bool USelectionChain::IsSelectionClassInFocusTrail(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	if (auto index = FindSelectionClassIndex(selectionClass)) {
		if (auto focusedIndex = FindSelectionClassIndex(mFocusedSelectionClass)) {
			return index.GetValue() <= focusedIndex.GetValue();
		}		
	}
	return false;
}
