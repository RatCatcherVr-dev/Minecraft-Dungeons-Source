#include "Dungeons.h"
#include "CounterWidgetBase.h"

int UCounterWidgetBase::GetValue() const {
	return mValue.Get(0);
}

void UCounterWidgetBase::SetEmphasis(EUICounterEmphasis emphasis) {
	mEmphasis = emphasis;
	OnEmphasisChanged(emphasis);
}

EUICounterEmphasis UCounterWidgetBase::GetEmphasis() const {
	return mEmphasis;
}

void UCounterWidgetBase::SetValue(int newValue) {
	const int lastValue = mValue.Get(newValue);
	mValue = newValue;
	OnValueChanged(newValue, lastValue);
}

void UCounterWidgetBase::ResetValue() {
	mValue.Reset();
	OnValueChanged(0, 0);
}
