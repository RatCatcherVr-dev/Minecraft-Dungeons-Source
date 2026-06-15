#include "Dungeons.h"
#include "GoldCurrency.h"

FSerializableItemId UGoldCurrency::GetCurrencyItemId() const {
	return game::item::type::Gold.getId();
}
