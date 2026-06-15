#include "CommonProperties.h"
#include "PropertyFileCache.h"
#include <array>
#include <chrono>
#include <random>
#include <StringConv.h>
#include <Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Guid.h>


CommonProperties::CommonProperties(FString cacheFilePath) : mCacheFilePath{cacheFilePath}
{

}

static FString GenerateUUID()
{
   FGuid guid;
   FGenericPlatformMisc::CreateGuid(guid);
   return guid.ToString();
}

void CommonProperties::LoadFromCache()
{
   PropertyFileCache pfc{ mCacheFilePath };
   PropertyList cachedProperties{};
   if (!pfc.LoadCache()) {
      auto installID = AnalyticsProperty{ "InstallID", GenerateUUID() };
      cachedProperties.Add("InstallID", installID);

      pfc.AddProperty(installID);
      pfc.SaveCache(); // This can fail. Not sure how to handle it yet. 
   }
   else {
      cachedProperties = pfc.GetProperties();
   }
   mCommonProperties.Append(cachedProperties);
}

void CommonProperties::GeneratePlatformProperties()
{
   mCommonProperties.Add("DeviceFamily", AnalyticsProperty{ "DeviceFamily", UGameplayStatics::GetPlatformName() });
   mCommonProperties.Add("OSVersion", AnalyticsProperty{ "OSVersion", FPlatformMisc::GetOSVersion() });
}

static FString GetLocale()
{
   auto userLocale = std::locale("").name();
   if (!userLocale.empty()) {
      return UTF8_TO_TCHAR(userLocale.c_str());
   }

   auto defaultLocale = std::locale().name();
   if (!defaultLocale.empty()) {
      return UTF8_TO_TCHAR(defaultLocale.c_str());
   }

   auto result = FPlatformMisc::GetDefaultLocale();
   if (result.IsEmpty())
      return "C";

   return result;
}

void CommonProperties::InitializeCommonProperties()
{
   LoadFromCache();

#if defined(_WIN32)
   mCommonProperties.Add("BuildPlatform", AnalyticsProperty{ "BuildPlatform", "win32" });
#elif defined(__APPLE__)
   mCommonProperties.Add("BuildPlatform", AnalyticsProperty{ "BuildPlatform", "mac" });
#elif defined(__linux__)
   mCommonProperties.Add("BuildPlatform", AnalyticsProperty{ "BuildPlatform", "linux" });
#endif
   GeneratePlatformProperties();
   mCommonProperties.Add("SysLocale", AnalyticsProperty{ "SysLocale", GetLocale() });

#ifndef NDEBUG
   mCommonProperties.Add("BuildTypeID", AnalyticsProperty{ "BuildTypeID", "debug" });
#endif
}
