#pragma once

#include <algorithm>

#include "PlayfabServicesWildCardValue.h"


class AnalyticsMeasurement 
{
public:
   enum AggregationType {
      Increment = 0,
      Sum,
      Min,
      Max,
      Average
   };

   AnalyticsMeasurement() {}

   AnalyticsMeasurement(const FString& measureName, AggregationType type, const PlayfabServicesWildCardValue& value)
      : mName(measureName)
      , mValue(value)
      , mValueDivisorForAverage(1)
      , mType(type) {
   }

   void updateMeasurement(const AnalyticsMeasurement& newMeasure) {
      switch (mType) {
      case AggregationType::Increment:
            mValue++;
            break;
      case AggregationType::Sum:
            mValue += newMeasure.mValue;
            break;
      case AggregationType::Min:
            mValue.AssignIfSmaller(newMeasure.mValue);
            break;
      case AggregationType::Max:
            mValue.AssignIfLarger(newMeasure.mValue);
            break;
      case AggregationType::Average:
            mValue += newMeasure.mValue;
            ++mValueDivisorForAverage;
            break;
      default:
            break;
      }
   }

   const FString& getName() const {
      return mName;
   }

   void setValue(PlayfabServicesWildCardValue value) {
      mValue = value;
   }
   
   PlayfabServicesWildCardValue getValue() const {
      if (mType == Average) {
            if (mValueDivisorForAverage != 0) {
               PlayfabServicesWildCardValue result(mValue.asFloat() / (float)mValueDivisorForAverage);
               return result;
            }
            else {
               return PlayfabServicesWildCardValue(0);
            }
      }
      else {
            return mValue;
      }
   }

   AggregationType getAggregationType() const {
      return mType;
   }

private:
   FString mName;
   PlayfabServicesWildCardValue mValue;
   int mValueDivisorForAverage;
   AggregationType mType;
};

