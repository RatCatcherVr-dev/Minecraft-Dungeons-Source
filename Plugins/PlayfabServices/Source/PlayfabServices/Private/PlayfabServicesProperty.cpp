#include "PlayfabServicesProperty.h"

PlayfabServicesProperty::PlayfabServicesProperty() {}

PlayfabServicesProperty::PlayfabServicesProperty(const FString& propName, const bool& value)
    : mName(propName)
    , mValue(value) {
}

PlayfabServicesProperty::PlayfabServicesProperty(const FString& propName, const float& value)
    : mName(propName)
    , mValue(value) {
}

PlayfabServicesProperty::PlayfabServicesProperty(const FString& propName, const int32_t& value)
    : mName(propName)
    , mValue(value) {
}

PlayfabServicesProperty::PlayfabServicesProperty(const FString& propName, const uint32_t& value)
    : mName(propName)
    , mValue(value) {
}

PlayfabServicesProperty::PlayfabServicesProperty(const FString& propName, const FString& value)
    : mName(propName)
    , mValue(value) {
}

const FString& PlayfabServicesProperty::getName() const {
    return mName;
}

void PlayfabServicesProperty::setValue(const bool& value) {
    mValue.setValue(value);
}

void PlayfabServicesProperty::setValue(const float& value) {
    mValue.setValue(value);
}

void PlayfabServicesProperty::setValue(const int32_t& value) {
    mValue.setValue(value);
}

void PlayfabServicesProperty::setValue(const uint32_t& value) {
    mValue.setValue(value);
}

void PlayfabServicesProperty::setValue(const FString& value) {
    mValue.setValue(value);
}

const PlayfabServicesWildCardValue& PlayfabServicesProperty::getValue() const {
    return mValue;
}
