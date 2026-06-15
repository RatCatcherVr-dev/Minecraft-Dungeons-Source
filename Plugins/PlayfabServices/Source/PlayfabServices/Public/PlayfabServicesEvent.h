#pragma once

#include "Math/Vector.h"
#include "PlayfabServicesMeasurement.h"
#include "PlayfabServicesProperty.h"
#include "UObject/Object.h"
#include <map>
#include <set>

typedef std::map<FString, PlayfabServicesProperty> PlayfabServicesPropertyList;
typedef std::map<FString, PlayfabServicesMeasurement> PlayfabServicesMeasurementList;

class UEnum;
class FName;

class PlayfabServicesEvent {
public:
    PLAYFABSERVICES_API PlayfabServicesEvent(const FString& userId, const FString& eventName);
	PLAYFABSERVICES_API PlayfabServicesEvent();

    PLAYFABSERVICES_API void addProperty(const PlayfabServicesProperty& prop);
    PLAYFABSERVICES_API void addProperty(const FString& propertyName, const bool& value);
    PLAYFABSERVICES_API void addProperty(const FString& propertyName, const float& value);
    PLAYFABSERVICES_API void addProperty(const FString& propertyName, const int32_t& value);
    PLAYFABSERVICES_API void addProperty(const FString& propertyName, const uint32_t& value);
    PLAYFABSERVICES_API void addProperty(const FString& propertyName, const char* value);
    PLAYFABSERVICES_API void addProperty(const FString& propertyName, const FString& value);
    PLAYFABSERVICES_API void addVectorProperties(const FString& propertyBaseName, const FVector& vectorValue);
    PLAYFABSERVICES_API void addEnumProperty(const FString& propertyName, const UEnum* enumType, const uint8_t& value);
    PLAYFABSERVICES_API void addObjectUProperties(const FString& propertyBaseName, const UObject* objectInstance, bool recursive=false);

    PLAYFABSERVICES_API void addMeasurement(const PlayfabServicesMeasurement& measure);
    PLAYFABSERVICES_API void addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const bool& value);
    PLAYFABSERVICES_API void addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const float& value);
    PLAYFABSERVICES_API void addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const int32_t& value);
    PLAYFABSERVICES_API void addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const uint32_t& value);

    PLAYFABSERVICES_API const FString& getName() const;

    PLAYFABSERVICES_API void setShouldAggregate(bool value);
    PLAYFABSERVICES_API bool getShouldAggregate() const;

    PLAYFABSERVICES_API uint32_t getAggregationTime() const;
    PLAYFABSERVICES_API void setAggregationTime(uint32_t seconds);

    PLAYFABSERVICES_API bool getProcessedRealtime() const;
    PLAYFABSERVICES_API void setProcessedRealtime(bool processedRealtime);

    PLAYFABSERVICES_API const FString& getUserId() const;

    PLAYFABSERVICES_API const PlayfabServicesPropertyList& getProperties() const;

    PLAYFABSERVICES_API const PlayfabServicesMeasurementList& getMeasurements() const;

private:
    void addUObjectPropertiesRecursive(const FString& propertyBaseName, const UObject* objectInstance, std::set<const UObject*>& circularGuard, bool recursive = false);

    FString mUserId;

    FString mName;
    bool mShouldAggregate;
    uint32_t mAggregationTimeSeconds;

    PlayfabServicesPropertyList mProperties;
    PlayfabServicesMeasurementList mMeasurements;
    bool mProcessedRealtime;
};
