#include "OfferEnchantmentPoints.h"
#include "game/merchant/MerchantSession.h"
#include "game/component/ItemStashComponent.h"


UOfferEnchantmentPoints* UOfferEnchantmentPoints::CreateSubobject(UObject* object, const FName& name, const MaxProvider& maxProvider) {
	auto offer = object->CreateDefaultSubobject<UOfferEnchantmentPoints>(name);
	offer->mMaxProvider = maxProvider;	
	return offer;
}

void UOfferEnchantmentPoints::OnSetupSession() {
	if (auto itemstash = GetSession().GetItemStashComponentMutable()) {
		itemstash->OnAvailableEnchangmentPointsChangedInternal.AddUObject(this, &UOfferEnchantmentPoints::AvailableEnchantmentPointsChanged);
	}
}

void UOfferEnchantmentPoints::AvailableEnchantmentPointsChanged() const {
	SelectionSelectableChanged();
}

void UOfferEnchantmentPoints::OnCleanupSession() {
	if (auto itemstash = GetSession().GetItemStashComponentMutable()) {
		itemstash->OnAvailableEnchangmentPointsChangedInternal.RemoveAll(this);
	}
}

void UOfferEnchantmentPoints::Refresh() {
	auto currentMax = GetMaxOfferableEnchantmentPoints();
	if (currentMax != mLastMax) {
		AvailableEnchantmentPointsChanged();
		if (mOfferedEnchantmentPoints > currentMax) {
			SetOfferedEnchantmentPoints(currentMax);
		}
		mLastMax = currentMax;
	}
}

void UOfferEnchantmentPoints::SetOfferedEnchantmentPoints(int points) {
	mOfferedEnchantmentPoints = points;
	SelectionChanged();
}

int UOfferEnchantmentPoints::GetMaxOfferableEnchantmentPoints() const {
	if (auto itemstash = GetSession().GetItemStashComponent()) {	
		auto currentMax = FMath::Max(0,itemstash->AvailableEnchantmentPoints());
		if (mMaxProvider) {
			currentMax = mMaxProvider(currentMax);
		}
		return currentMax;
	}
	return 0;
}

int UOfferEnchantmentPoints::GetOfferedEnchantmentPoints() const {
	return mOfferedEnchantmentPoints;
}

int UOfferEnchantmentPoints::GetAvailableOfferedEnchantmentPoints() const {
	return FMath::Max(0,FMath::Min(GetMaxOfferableEnchantmentPoints(), GetOfferedEnchantmentPoints()));
}

bool UOfferEnchantmentPoints::ShouldShowEnchantmentPoints() const {
	return true;
}

void UOfferEnchantmentPoints::EnsureSelection() {	
	SetOfferedEnchantmentPoints(FMath::Min(GetMaxOfferableEnchantmentPoints(), 1));
}

void UOfferEnchantmentPoints::ClearSelection() {
	SetOfferedEnchantmentPoints(0);
}

bool UOfferEnchantmentPoints::CanSelectAny() const {
	return true;
}

bool UOfferEnchantmentPoints::HasSelectedAny() const {
	return mOfferedEnchantmentPoints != 0;
}

