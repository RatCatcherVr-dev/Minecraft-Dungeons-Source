#pragma once
#include "AnalyticsEvent.h"
#include <memory>
#include <UnrealString.h>

/*
   Properties common to all event messages.

   Usage:

   //Instantiate and initialize class.
   CommonProperties commonProperties();
   commonProperties.InitializeCommonProperties();

   //To get common properties
   commonProperties.GetProperties();
*/
class CommonProperties {
public:
   CommonProperties(FString cacheFilePath);
   ~CommonProperties() = default;
      
   void InitializeCommonProperties();
   const PropertyList& GetProperties() const { return mCommonProperties; }
private:
   void LoadFromCache();
   void GeneratePlatformProperties();

   PropertyList mCommonProperties{};
   FString mCacheFilePath{};

};
