#include "Dungeons.h"
#include "EnchantmentPointsCounterWidget.h"
#include "game/component/ItemStashComponent.h"

TOptional<int> UEnchantmentPointsCounterWidget::FetchBoundValue(const AActor&) const {
	if (mitemStash) {
		return mitemStash->AvailableEnchantmentPoints();
	}
	return {};
}

void UEnchantmentPointsCounterWidget::BindTo(AActor& actor) {
	if (auto itemStash = actor.FindComponentByClass<UItemStashComponent>()) {
		itemStash->OnAvailableEnchangmentPointsChangedInternal.AddUObject(this, &UEnchantmentPointsCounterWidget::Refresh);
		mitemStash = itemStash;
	}
}

void UEnchantmentPointsCounterWidget::UnbindFrom(AActor& actor) {
	if (mitemStash) {
		mitemStash->OnAvailableEnchangmentPointsChangedInternal.RemoveAll(this);
		mitemStash = nullptr;
	}
}
