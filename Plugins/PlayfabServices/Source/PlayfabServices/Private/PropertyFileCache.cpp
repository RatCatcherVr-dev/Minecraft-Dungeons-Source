#include "PropertyFileCache.h"

#include <fstream>
#include <PlatformFile.h>
#include <Runtime/Core/Public/HAL/PlatformFilemanager.h>
#include <Runtime/Core/Public/Containers/StringConv.h>
#include <Runtime/Core/Public/Misc/FileHelper.h>
#include <Runtime/Json/Public/Json.h>

PropertyFileCache::PropertyFileCache(const FString& file) : mFileName(file) {
}

static std::uint32_t ComputeChecksum(const FString& str)
{
   // This is sort of how zip computes checksums, simple and fast.
   constexpr auto POLY = 0xedb88320;

   auto crc = 0u;
   crc = ~crc;
   for (auto& c : str) {
      crc ^= c;
      for (auto bitIndex = 0; bitIndex < 8; ++bitIndex) {
         crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
      }
   }
   return ~crc;
}

bool PropertyFileCache::PopulateFromJson(const FString& jsonStr)
{
   TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonStr);
   TSharedPtr<FJsonObject> object;
   bool bSuccessful = FJsonSerializer::Deserialize(reader, object);
   if (!bSuccessful || !object.IsValid())
      return false;

   auto iter = object->Values.CreateIterator();
   while (iter)
   {
      switch (iter->Value->Type)
      {
      case EJson::Boolean:
         mCachedProperties.Add(iter->Key, AnalyticsProperty( iter->Key, PlayfabServicesWildCardValue(iter->Value->AsBool())));
         break;
      case EJson::Number:
         mCachedProperties.Add(iter->Key, AnalyticsProperty( iter->Key, PlayfabServicesWildCardValue((float)iter->Value->AsNumber())));
         break;
      case EJson::String:
         mCachedProperties.Add(iter->Key, AnalyticsProperty(iter->Key, PlayfabServicesWildCardValue(iter->Value->AsString())));
         break;
      }
      ++iter;
   }
   
   return true;
}

bool PropertyFileCache::LoadCache()
{
   IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
   if (!platformFile.FileExists(*mFileName))
   {
      return false;
   }
   
   FString fFileContents;
   if (!FFileHelper::LoadFileToString(fFileContents, *mFileName))
      return false;

   //Pluck the checksum off the end. 
   int32 endOfJson;
   fFileContents.FindLastChar('}', endOfJson);
   if (endOfJson == INDEX_NONE) {
      return false;
   }

   auto jsonStr = fFileContents.Left(endOfJson + 1);
   auto checksumStr = fFileContents.Right(fFileContents.Len() - endOfJson + 1);
   uint32 loadedChecksum = 0;
   LexFromString(loadedChecksum, *checksumStr);
   auto computedChecksum = ComputeChecksum(jsonStr);
   if (loadedChecksum != computedChecksum) {
      return false;
   }

   if (!PopulateFromJson(jsonStr))
      return false;

   return true;
}

void PropertyFileCache::SaveCache() const
{
   TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);

   for (auto& propertyPair : mCachedProperties)
   {
      switch (propertyPair.Value.getValue().getType())
      {
      case PlayfabServicesWildCardValue::ValueType::Boolean:
         jsonObject->SetBoolField(propertyPair.Key, propertyPair.Value.getValue().asBoolean());
         break;
      case PlayfabServicesWildCardValue::ValueType::Float:
         jsonObject->SetNumberField(propertyPair.Key, propertyPair.Value.getValue().asFloat());
         break;
      case PlayfabServicesWildCardValue::ValueType::SignedInt32:
         jsonObject->SetNumberField(propertyPair.Key, (double)propertyPair.Value.getValue().asInt32());
         break;
      case PlayfabServicesWildCardValue::ValueType::UnsignedInt32:
         jsonObject->SetNumberField(propertyPair.Key, (double)propertyPair.Value.getValue().asUInt32());
         break;
      case PlayfabServicesWildCardValue::ValueType::String:
         jsonObject->SetStringField(propertyPair.Key, propertyPair.Value.getValue().asString());
      }
   }

   FString OutputString;
   TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
   FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);

   auto checksum = ComputeChecksum(OutputString);
   OutputString.AppendInt(checksum);

   FFileHelper::SaveStringToFile(OutputString, *mFileName);
}
