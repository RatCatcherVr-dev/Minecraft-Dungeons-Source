#pragma once

#include "AnalyticsMeasurement.h"
#include "AnalyticsProperty.h"
#include <chrono>
#include <unordered_map>
#include <Runtime/Core/Public/Containers/Map.h>

typedef TMap<FString, AnalyticsProperty>  PropertyList;
typedef TMap<FString, AnalyticsMeasurement>  MeasurementList;

class AnalyticsEvent {
   public:
      AnalyticsEvent(const FString& userId, const FString& eventName);

      void stampWithRecord(uint32_t recordSequence);

      void addProperty(const AnalyticsProperty& prop) {
         mProperties.Add(prop.getName(), prop);
      }

      void addProperty(const FString& propertyName, const PlayfabServicesWildCardValue& value) {
         AnalyticsProperty prop(propertyName, value);
         mProperties.Add(propertyName, prop);
      }

      void addMeasurement(const AnalyticsMeasurement& measure) {
         mMeasurements.Add(measure.getName(), measure);
      }

      void addMeasurement(const FString& measureName, AnalyticsMeasurement::AggregationType aggregationType, const PlayfabServicesWildCardValue& value) {
         AnalyticsMeasurement measure(measureName, aggregationType, value);
         mMeasurements.Add(measureName, measure);
      }

      bool operator==(const AnalyticsEvent& other) const;
      void updateMeasurements(const AnalyticsEvent& evnt);

      const FString& getName() const {
         return mName;
      }

      void setShouldAggregate(bool value) {
         mShouldAggregate = value;
      }

      bool getShouldAggregate() const {
         return mShouldAggregate;
      }

      void setAggregationTime(uint32_t seconds) {
         mAggregationTimeSeconds = seconds;
      }

      bool getProcessedRealtime() const {
         return mProcessedRealtime;
      }

      void setProcessedRealtime(bool processedRealtime) {
         mProcessedRealtime = processedRealtime;
      }

      std::chrono::time_point<std::chrono::system_clock> getAggregationCompleteTime() const {
         return mEventCreationTime + std::chrono::seconds(mAggregationTimeSeconds);
      }

      const FString& getUserId() const;

      const PropertyList& getProperties() const {
         return mProperties;
      }

      const MeasurementList& getMeasurements() const {
         return mMeasurements;
      }
   private:
	   const FString mUserId;
      const FString mName;
      bool mShouldAggregate;
      uint32_t mAggregationTimeSeconds;
      std::chrono::time_point<std::chrono::system_clock> mEventCreationTime;

      PropertyList mProperties;
      MeasurementList mMeasurements;

      bool mRecordStamped;
      bool mProcessedRealtime;
   };

