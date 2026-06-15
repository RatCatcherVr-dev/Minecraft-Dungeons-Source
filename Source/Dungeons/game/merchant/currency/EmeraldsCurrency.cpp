#include "Dungeons.h"
#include "EmeraldsCurrency.h"

FSerializableItemId UEmeraldsCurrency::GetCurrencyItemId() const {
	return game::item::type::Emerald.getId();
}
