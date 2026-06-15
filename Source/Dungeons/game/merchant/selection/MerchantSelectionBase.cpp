#include "Dungeons.h"
#include "game/merchant/MerchantContext.h"
#include "game/merchant/type/MerchantBase.h"
#include "MerchantSelectionBase.h"

void UMerchantSelectionBase::SelectionChanged() const {
	OnSelectionChangedInternal.Broadcast(*this);
	OnSelectionChanged.Broadcast();
	OnSelectionChangedInternalAfterBlueprints.Broadcast(*this);
}

void UMerchantSelectionBase::SelectionSelectableChanged() const {
	OnMerchantSelectionSelectableChanged.Broadcast();
}

void UMerchantSelectionBase::OnSetupSession() {
	Super::OnSetupSession();
	SelectionSelectableChanged();
}

bool UMerchantSelectionBase::TryCancelSelection() {
	if (HasSelectedAny()) {
		ClearSelection();
		OnSelectionCancelledInternal.Broadcast(*this);
		return true;
	}
	return false;
}

bool UMerchantSelectionBase::TryConfirmSelection() {
	EnsureSelection();
	if(HasSelectedAny()){
		OnSelectionConfirmedInternal.Broadcast(*this);
		return true;
	}
	return false;
}

bool UMerchantSelectionBase::CanCancelSelection() const {
	return GetContext().GetMerchant().CanCancelSelection(GetClass());
}

bool UMerchantSelectionBase::CanConfirmSelection() const {
	return GetContext().GetMerchant().CanConfirmSelection(GetClass());
}

bool UMerchantSelectionBase::IsFocused() const {
	return GetContext().GetMerchant().IsSelectionClassFocused(GetClass());
}
