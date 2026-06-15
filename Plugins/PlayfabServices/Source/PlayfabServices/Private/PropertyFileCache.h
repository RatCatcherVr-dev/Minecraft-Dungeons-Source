#pragma once
#include <string>
#include "AnalyticsEvent.h"
#include <memory>

/* 
   PropertyFileCache managed properties which are cached to disk. To instantiate
   this class, a file name and an IFileIO implementation must be provided. See
   IFileIO.h for details on defining a file i/o interface.
*/
class PropertyFileCache {
public:
   PropertyFileCache(const FString& file);
   ~PropertyFileCache() = default;

   bool LoadCache();
   void SaveCache() const;
      
   const PropertyList& GetProperties() const { return mCachedProperties; }
   void AddProperty(const AnalyticsProperty& p) { mCachedProperties.Add(p.getName(), p); }

private:
   bool PopulateFromJson(const FString& jsonStr);

   FString mFileName{ "" };
   PropertyList mCachedProperties{};
};
