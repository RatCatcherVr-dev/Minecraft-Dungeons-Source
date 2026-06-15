#include "Dungeons.h"
#include "ItemPowerUtil.h"

float UItemPowerUtil::GetItemPowerDisplayValueFloat(float ItemPower) {
	const float PowerAboveOne = FMath::Max(ItemPower - 1.0f, 0.0f);
	const float ScaledPower = PowerAboveOne * 10.0f;
	const float ScaledPower1Indexed = ScaledPower + 1;
	return ScaledPower1Indexed;
}

int UItemPowerUtil::GetItemPowerDisplayInt(float ItemPower) {
	return FMath::FloorToInt(GetItemPowerDisplayValueFloat(ItemPower));
}

FText UItemPowerUtil::GetItemPowerDisplayText(float ItemPower) {
	const int PowerInt = GetItemPowerDisplayInt(ItemPower);
	return FText::FromString(FString::FromInt(PowerInt));
}

float UItemPowerUtil::GetItemPowerFromDisplayValue(float DisplayItemPower) {
	const float ScaledPower0Indexed = DisplayItemPower - 1;
	const float PowerAboveOne = ScaledPower0Indexed / 10;
	return PowerAboveOne + 1;
}
