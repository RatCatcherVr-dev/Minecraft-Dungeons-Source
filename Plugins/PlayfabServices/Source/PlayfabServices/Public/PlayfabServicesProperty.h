#pragma once

#include "PlayfabServicesWildCardValue.h"

class UObject;
class UClass;

class PlayfabServicesProperty {
public:
    PLAYFABSERVICES_API PlayfabServicesProperty();
    PLAYFABSERVICES_API PlayfabServicesProperty(const FString& propName, const bool& value);
    PLAYFABSERVICES_API PlayfabServicesProperty(const FString& propName, const float& value);
    PLAYFABSERVICES_API PlayfabServicesProperty(const FString& propName, const int32_t& value);
    PLAYFABSERVICES_API PlayfabServicesProperty(const FString& propName, const uint32_t& value);
    PLAYFABSERVICES_API PlayfabServicesProperty(const FString& propName, const FString& value);

    PLAYFABSERVICES_API const FString& getName() const;

    PLAYFABSERVICES_API void setValue(const bool& value);
    PLAYFABSERVICES_API void setValue(const float& value);
    PLAYFABSERVICES_API void setValue(const int32_t& value);
    PLAYFABSERVICES_API void setValue(const uint32_t& value);
    PLAYFABSERVICES_API void setValue(const FString& value);

    PLAYFABSERVICES_API const PlayfabServicesWildCardValue& getValue() const;

private:
    FString mName;
    PlayfabServicesWildCardValue mValue;
};
