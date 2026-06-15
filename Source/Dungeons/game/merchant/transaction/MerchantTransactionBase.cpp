#include "Dungeons.h"
#include "game/merchant/currency/MerchantCurrencyComponent.h"
#include "MerchantTransactionBase.h"
#include "game/merchant/MerchantContext.h"
#include "game/merchant/type/MerchantBase.h"

void UMerchantTransactionBase::TransactionChanged() const {	
	OnMerchantTransactionChanged.Broadcast();
}

void UMerchantTransactionBase::Refresh() {
	TransactionChanged();	
}

FMerchantDisplayPrice UMerchantTransactionBase::GetPrice() const {
	if (auto currency = GetSession().GetCurrencyComponent()) {
		return currency->AsMerchantDisplayPrice(Validate().GetOptionalPrice());
	}
	return {};
}

bool UMerchantTransactionBase::HasPrice() const {
	return Validate().GetOptionalPrice().IsSet();
}

EMerchantTransactionStatusReason UMerchantTransactionBase::GetStatusReason() const {
	return Validate().GetOptionalStatus().Get(EMerchantTransactionStatusReason::UNSET);
}

TArray<FItemBulletPoint> UMerchantTransactionBase::GetStatusBulletPoints() const {
	return Validate().GetBulletPoints();
}

FProblemStatus UMerchantTransactionBase::QueryProblemStatus(bool& hasProblem) const {
	auto validated = Validate();
	if (auto maybeProblem = validated.GetOptionalProblemStatus()) {
		hasProblem = true;
		return maybeProblem.GetValue();
	}
	hasProblem = false;
	return {};
}

bool UMerchantTransactionBase::TryExecute() {	
	auto status = Validate();
	if(status.CanExecute()){
		status.Execute();
		OnMerchantTransactionExecutedInternal.Broadcast(*this);
		return true;
	}
	return false;
}

bool UMerchantTransactionBase::CanExecute() const {
	auto status = Validate();
	return status.CanExecute();
}

bool UMerchantTransactionBase::CanSelectAllRequiredSelections() const {
	return GetContext().GetMerchant().CanTransactionSelectAllRequiredSelections(this);
}

TSet<TSubclassOf<UMerchantSelectionBase>> UMerchantTransactionBase::GetRequiredSelectionClasses() const {
	TSet<TSubclassOf<UMerchantSelectionBase>> selections;
	PopulateRequiredSelectionClasses(selections);
	return selections;
}
