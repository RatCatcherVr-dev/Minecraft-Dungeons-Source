#pragma once

#include "PlayfabServicesWildCardValue.h"

class PlayfabServicesMeasurement {
public:
    enum AggregationType {
        Increment = 0,
        Sum,
        Min,
        Max,
        Average
    };

    PLAYFABSERVICES_API PlayfabServicesMeasurement();
    PLAYFABSERVICES_API PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const bool& value);
    PLAYFABSERVICES_API PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const float& value);
    PLAYFABSERVICES_API PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const int32_t& value);
    PLAYFABSERVICES_API PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const uint32_t& value);
    PLAYFABSERVICES_API PlayfabServicesMeasurement(const FString& measureName, AggregationType type, const FString& value);

    PLAYFABSERVICES_API const FString& getName() const;

    PLAYFABSERVICES_API void setValue(const bool& value);
    PLAYFABSERVICES_API void setValue(const float& value);
    PLAYFABSERVICES_API void setValue(const int32_t& value);
    PLAYFABSERVICES_API void setValue(const uint32_t& value);
    PLAYFABSERVICES_API void setValue(const FString& value);

    PLAYFABSERVICES_API const PlayfabServicesWildCardValue& getValue() const;

    PLAYFABSERVICES_API AggregationType getAggregationType() const;

private:
    FString mName;
    PlayfabServicesWildCardValue mValue;
    AggregationType mType;
};

