#pragma once

#include "PlayfabServicesWildCardValue.h"

class AnalyticsProperty {
public:
   AnalyticsProperty() {}

   AnalyticsProperty(const FString& propertyName, const PlayfabServicesWildCardValue& value)
      : mName(propertyName)
      , mValue(value) {

   }

   bool operator==(const AnalyticsProperty& other) const {
      if (mName != other.getName() || mValue.getType() != other.getValue().getType()) {
            return false;
      }

      return mValue == other.getValue();
   }
        
   const FString &getName() const {
      return mName;
   }

   void setValue(PlayfabServicesWildCardValue value) {
      mValue = value;
   }

   const PlayfabServicesWildCardValue& getValue() const {
      return mValue;
   }

private:
   FString mName;
   PlayfabServicesWildCardValue mValue;
};
