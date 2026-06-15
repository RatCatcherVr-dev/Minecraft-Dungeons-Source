#include "PlayfabServicesWildCardValue.h"
#include <sstream>

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue()
    : mType(ValueType::NotSet) {
}

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue(bool value)
    : mType(ValueType::Boolean) {
    mValue.mBoolean = value;
}

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue(float value)
    : mType(ValueType::Float) {
    mValue.mFloat = value;
}

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue(int32_t value)
    : mType(ValueType::SignedInt32) {
    mValue.mSignedInt = value;
}

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue(uint32_t value)
    : mType(ValueType::UnsignedInt32) {
    mValue.mUnsignedInt = value;
}

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue(const char* value)
	: PlayfabServicesWildCardValue(FString(value)) {
}

PlayfabServicesWildCardValue::PlayfabServicesWildCardValue(const FString& value)
    : mString(value)
    , mType(ValueType::String)
{
}

void PlayfabServicesWildCardValue::setValue(bool value) {
    mType = ValueType::Boolean;
    mValue.mBoolean = value;
}

void PlayfabServicesWildCardValue::setValue(float value) {
    mType = ValueType::Float;
    mValue.mFloat = value;
}

void PlayfabServicesWildCardValue::setValue(int32_t value) {
    mType = ValueType::SignedInt32;
    mValue.mSignedInt = value;
}

void PlayfabServicesWildCardValue::setValue(uint32_t value) {
    mType = ValueType::UnsignedInt32;
    mValue.mUnsignedInt = value;
}

void PlayfabServicesWildCardValue::setValue(const char* value) {
	setValue(FString(value));
}

void PlayfabServicesWildCardValue::setValue(const FString& value) {
    mType = ValueType::String;
    mString = value;
}

PlayfabServicesWildCardValue::ValueType PlayfabServicesWildCardValue::getType() const {
    return mType;
}

bool PlayfabServicesWildCardValue::asBoolean() const {
    bool result;
    switch (getType()) {
    case ValueType::Boolean:
        result = mValue.mBoolean;
        break;
    case ValueType::Float:
        result = static_cast<bool>(mValue.mFloat);
        break;
    case ValueType::SignedInt32:
        result = static_cast<bool>(mValue.mSignedInt);
        break;
    case ValueType::UnsignedInt32:
        result = static_cast<bool>(mValue.mUnsignedInt);
        break;
    case ValueType::String:
        result = mString != "false";
        break;
    default:
        result = false;
        break;
    }
    return result;
}

float PlayfabServicesWildCardValue::asFloat() const {
    float result;
    switch (getType()) {
    case ValueType::Boolean:
        result = mValue.mBoolean ? 1.0f : 0.0f;
        break;
    case ValueType::Float:
        result = mValue.mFloat;
        break;
    case ValueType::SignedInt32:
        result = static_cast<float>(mValue.mSignedInt);
        break;
    case ValueType::UnsignedInt32:
        result = static_cast<float>(mValue.mUnsignedInt);
        break;
    case ValueType::String:
        result = 0.0f;
        break;
    default:
        result = 0.0f;
        break;
    }
    return result;
}

int32_t PlayfabServicesWildCardValue::asInt32() const {
    int32_t result;
    switch (getType()) {
    case ValueType::Boolean:
        result = mValue.mBoolean ? 1 : 0;
        break;
    case ValueType::Float:
        result = static_cast<int32_t>(mValue.mFloat);
        break;
    case ValueType::SignedInt32:
        result = mValue.mSignedInt;
        break;
    case ValueType::UnsignedInt32:
        result = static_cast<int32_t>(mValue.mUnsignedInt);
        break;
    case ValueType::String:
        result = 0;
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

uint32_t PlayfabServicesWildCardValue::asUInt32() const {
    uint32_t result;
    switch (getType()) {
    case ValueType::Boolean:
        result = mValue.mBoolean ? 1U : 0U;
        break;
    case ValueType::Float:
        result = static_cast<uint32_t>(mValue.mFloat);
        break;
    case ValueType::SignedInt32:
        result = static_cast<uint32_t>(mValue.mSignedInt);
        break;
    case ValueType::UnsignedInt32:
        result = mValue.mUnsignedInt;
        break;
    case ValueType::String:
        result = 0U;
        break;
    default:
        result = 0U;
        break;
    }
    return result;
}

FString PlayfabServicesWildCardValue::asString() const {
    switch (getType()) {
    case ValueType::Boolean:
       return LexToString(mValue.mBoolean);
       break;
    case ValueType::Float:
       return LexToString(mValue.mFloat);
       break;
    case ValueType::SignedInt32:
       return LexToString(mValue.mSignedInt);
       break;
    case ValueType::UnsignedInt32:
       return LexToString(mValue.mUnsignedInt);
       break;
    case ValueType::String:
        return mString;
    default:
       break;
    }
    return "";
}
