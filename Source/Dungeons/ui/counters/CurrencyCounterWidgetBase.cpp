#include "Dungeons.h"
#include "CurrencyCounterWidgetBase.h"
#include "game/component/WalletComponent.h"
#include "game/item/SerializableItemId.h"
#include "DungeonsGameInstance.h"

void UCurrencyCounterWidgetBase::OnWalletCurrencyChanged(const FSerializableItemId& itemId) {
	if (IsInterrestedInCurrency(itemId)) {
		Refresh();
	}
}

TOptional<int> UCurrencyCounterWidgetBase::FetchBoundValue(const AActor& actor) const {
	if (mWallet) {
		return GetWalletBalance(*mWallet);
	}
	return {};
}

void UCurrencyCounterWidgetBase::BindTo(AActor& actor) {
	if (auto* wallet = actor.FindComponentByClass<UWalletComponent>()) {
		wallet->OnDisplayCountChangedInternal.AddUObject(this, &UCurrencyCounterWidgetBase::OnWalletCurrencyChanged);
		mWallet = wallet;
	}
}

void UCurrencyCounterWidgetBase::UnbindFrom(AActor& actor) {
	if (mWallet) {
		mWallet->OnDisplayCountChangedInternal.RemoveAll(this);
		mWallet = nullptr;
	}
}

bool UCurrencyCounterWidgetBase::IsInterrestedInCurrency(const FSerializableItemId& itemId) const {
	return CurrencyItemId == itemId;
}

int UCurrencyCounterWidgetBase::GetWalletBalance(const UWalletComponent& wallet) const {
	return wallet.Balance(CurrencyItemId);
}

void UCurrencyCounterWidgetBase::SetCurrencyType(const FSerializableItemId& itemId) {
	CurrencyItemId = itemId;
	Refresh();
	OnCurrencyTypeChanged(itemId);
}

const FSerializableItemId& UCurrencyCounterWidgetBase::GetCurrencyItemId() const {
	return CurrencyItemId;
}

bool UCurrencyCounterWidgetBase::IsCurrencyItemId(const FSerializableItemId& itemId) const {
	return IsInterrestedInCurrency(itemId);
}

bool UCurrencyCounterWidgetBase::HasCurrencyEntitlement(const UObject* worldContextObject) const
{
	if (!RequiredEntitlement.IsEmpty())
	{
		const auto* repository = worldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetEntitlementsRepository();
		return repository->GetEntitlement(RequiredEntitlement).IsSet();
	}

	return true;
}
