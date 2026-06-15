#include "Dungeons.h"
#include "game/actor/character/merchant/MerchantActor.h"
#include "MerchantWidgetBase.h"

void UMerchantWidgetBase::OnRequiredSelectionsChanged() {
	UpdateSelections();
}

void UMerchantWidgetBase::OnAvailableTransactionsChanged() {
	UpdateTransactions();
}

void UMerchantWidgetBase::OnSelectionFocusChanged() {
	UpdateSelectionFocus();
}

void UMerchantWidgetBase::OnMerchantTransactionExecuted(const UMerchantTransactionBase& transaction) {	
	OnTransactionExecuted(&transaction);
}

void UMerchantWidgetBase::BindTo(AMerchantBase* merchant) {
	if (mMerchant) {
		mMerchant->OnRequiredSelectionsChanged.RemoveAll(this);
		mMerchant->OnAvailableTransactionsChanged.RemoveAll(this);		
		mMerchant->OnSelectionFocusChanged.RemoveAll(this);
		mMerchant->OnItemAnnouncement.RemoveAll(this);
		mMerchant->OnMerchantTransactionExecuted.RemoveAll(this);
		OnUnboundMerchant(mMerchant);
	}
	mMerchant = merchant;
	if(merchant){
		mMerchant->OnBindingToWidget();
		mMerchant->OnRequiredSelectionsChanged.AddUObject(this, &UMerchantWidgetBase::OnRequiredSelectionsChanged);
		mMerchant->OnAvailableTransactionsChanged.AddUObject(this, &UMerchantWidgetBase::OnAvailableTransactionsChanged);
		mMerchant->OnSelectionFocusChanged.AddUObject(this, &UMerchantWidgetBase::OnSelectionFocusChanged);
		mMerchant->OnItemAnnouncement.AddUObject(this, &UMerchantWidgetBase::OnItemAnnouncement);
		mMerchant->OnMerchantTransactionExecuted.AddUObject(this, &UMerchantWidgetBase::OnMerchantTransactionExecuted);
		OnBoundMerchant(merchant);
	}
	OnBindChanged();
	UpdateSlots();
	UpdateSelections();
	UpdateTransactions();
}

bool UMerchantWidgetBase::IsBoundTo(AMerchantBase* merchant) const {
	return mMerchant == merchant;
}

bool UMerchantWidgetBase::IsTransactionClassAvailable(const TSubclassOf<UMerchantTransactionBase>& transactionClass) const {
	return mMerchant ? mMerchant->IsTransactionClassAvailable(transactionClass) : false;	
}

bool UMerchantWidgetBase::IsSelectionClassRequired(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return mMerchant ? mMerchant->IsSelectionClassRequired(selectionClass) : false;	
}

bool UMerchantWidgetBase::IsSelectionClassFocused(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return mMerchant ? mMerchant->IsSelectionClassFocused(selectionClass) : false;
}

USelectionChain* UMerchantWidgetBase::GetSelectionFocus() const {
	return mMerchant ? mMerchant->GetSelectionFocus() : nullptr;
}

UMerchantSelectionBase* UMerchantWidgetBase::GetSelectionByClass(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return mMerchant ? mMerchant->GetSelectionByClass(selectionClass) : nullptr;
}

UMerchantTransactionBase* UMerchantWidgetBase::GetTransactionByClass(const TSubclassOf<UMerchantTransactionBase>& transactionClass) const {
	return mMerchant ? mMerchant->GetTransactionByClass(transactionClass) : nullptr;
}

TArray<UMerchantSlotBase*> UMerchantWidgetBase::GetSlots() const {
	if (mMerchant) {
		return mMerchant->GetSlots();
	}
	return {};
}

TArray<UMerchantSlotBase*> UMerchantWidgetBase::GetSlotsByClass(const TSubclassOf<UMerchantSlotBase>& slotClass) const {
	return GetSlots().FilterByPredicate([slotClass](auto& slot) { return slot->IsA(slotClass); });
}

TArray<FItemBulletPoint> UMerchantWidgetBase::GetDisplayBulletPoints() const {
	if (mMerchant) {
		return mMerchant->GetDisplayBulletPoints();
	}
	return {};
}

bool UMerchantWidgetBase::HasAnyLockedSlot() const {
	return mMerchant ? mMerchant->HasAnyLockedSlots() : false;
}

bool UMerchantWidgetBase::ShouldShowEnchantmentPoints() const {
	return mMerchant ? mMerchant->ShouldShowEnchantmentPoints() : false;
}

TArray<FItemBulletPoint> UMerchantWidgetBase::GetRestockMethodsBulletPoints() const {
	if (mMerchant) {
		return mMerchant->GetRestockMethodsBulletPoints();
	}
	return {};
}

AMerchantActor* UMerchantWidgetBase::GetMerchantActorOwner() const {
	return mMerchant ? mMerchant->GetMerchantActorOwner() : nullptr;
}
 
UMerchantCurrencyComponent* UMerchantWidgetBase::GetCurrencyComponent() const {
	return mMerchant ? mMerchant->GetCurrencyComponent() : nullptr;		
}

const FText& UMerchantWidgetBase::GetDisplayDescription() const {
	return mMerchant ? mMerchant->GetDisplayDescription() : FText::GetEmpty();
}

const FText& UMerchantWidgetBase::GetDisplayName() const {
	return mMerchant ? mMerchant->GetDisplayName() : FText::GetEmpty();
}

const FText& UMerchantWidgetBase::GetLockedSlotsHint() const {
	return mMerchant ? mMerchant->GetLockedSlotsHint() : FText::GetEmpty();
}
