#include "PlayfabServicesMeasurement.h"

PlayfabServicesMeasurement::PlayfabServicesMeasurement() {}

PlayfabServicesMeasurement::PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const bool& value)
    : mName(measureName)
    , mValue(value)
    , mType(type)
{
}

PlayfabServicesMeasurement::PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const float& value)
    : mName(measureName)
    , mValue(value)
    , mType(type)
{
}

PlayfabServicesMeasurement::PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const int32_t& value)
    : mName(measureName)
    , mValue(value)
    , mType(type)
{
}

PlayfabServicesMeasurement::PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const uint32_t& value)
    : mName(measureName)
    , mValue(value)
    , mType(type)
{
}

PlayfabServicesMeasurement::PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const FString& value)
    : mName(measureName)
    , mValue(value)
    , mType(type)
{
}

const FString& PlayfabServicesMeasurement::getName() const {
    return mName;
}

void PlayfabServicesMeasurement::setValue(const bool& value)
{
    mValue.setValue(value);
}

void PlayfabServicesMeasurement::setValue(const float& value)
{
    mValue.setValue(value);
}

void PlayfabServicesMeasurement::setValue(const int32_t& value)
{
    mValue.setValue(value);
}

void PlayfabServicesMeasurement::setValue(const uint32_t& value)
{
    mValue.setValue(value);
}

void PlayfabServicesMeasurement::setValue(const FString& value)
{
    mValue.setValue(value);
}

const PlayfabServicesWildCardValue& PlayfabServicesMeasurement::getValue() const {
    return mValue;
}

PlayfabServicesMeasurement::AggregationType PlayfabServicesMeasurement::getAggregationType() const {
    return mType;
}
