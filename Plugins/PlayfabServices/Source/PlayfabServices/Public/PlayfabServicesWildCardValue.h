#pragma once

#include <string>
#include <UnrealString.h>
#include <algorithm>

class PlayfabServicesWildCardValue {
public:

    enum class ValueType : uint16_t {
        Boolean,
        Float,
        SignedInt32,
        UnsignedInt32,
        String,
        NotSet
    };

    PLAYFABSERVICES_API PlayfabServicesWildCardValue();
    PLAYFABSERVICES_API PlayfabServicesWildCardValue(bool value);
    PLAYFABSERVICES_API PlayfabServicesWildCardValue(float value);
    PLAYFABSERVICES_API PlayfabServicesWildCardValue(int32_t value);
    PLAYFABSERVICES_API PlayfabServicesWildCardValue(uint32_t value);
    PLAYFABSERVICES_API PlayfabServicesWildCardValue(const char* value);
    PLAYFABSERVICES_API PlayfabServicesWildCardValue(const FString& value);

    PLAYFABSERVICES_API void setValue(bool value);
    PLAYFABSERVICES_API void setValue(float value);
    PLAYFABSERVICES_API void setValue(int32_t value);
    PLAYFABSERVICES_API void setValue(uint32_t value);
    PLAYFABSERVICES_API void setValue(const char* value);
    PLAYFABSERVICES_API void setValue(const FString& value);

    PLAYFABSERVICES_API ValueType getType() const;

    PLAYFABSERVICES_API bool asBoolean() const;
    PLAYFABSERVICES_API float asFloat() const;
    PLAYFABSERVICES_API int32_t asInt32() const;
    PLAYFABSERVICES_API uint32_t asUInt32() const;
    PLAYFABSERVICES_API FString asString() const;

    PlayfabServicesWildCardValue& operator++()
    {
       switch (mType)
       {
       case ValueType::SignedInt32:
          mValue.mSignedInt++;
          break;
       case ValueType::UnsignedInt32:
          mValue.mUnsignedInt++;
          break;
       case ValueType::Float:
          mValue.mFloat += 1.0f;
          break;
       }
       return *this;
    }

    PlayfabServicesWildCardValue operator++(int)
    {
       PlayfabServicesWildCardValue tmp(*this); 
       operator++(); 
       return tmp;   
    }

    PlayfabServicesWildCardValue& operator+=(const PlayfabServicesWildCardValue& rhs) 
    {
       //Types must match. No conversions here.
       if (mType != rhs.mType)
          return *this;

       switch (mType)
       {
       case ValueType::SignedInt32:
          mValue.mSignedInt += rhs.mValue.mSignedInt;
          break;
       case ValueType::UnsignedInt32:
          mValue.mUnsignedInt += rhs.mValue.mUnsignedInt;
          break;
       case ValueType::Float:
          mValue.mFloat += rhs.mValue.mFloat;
          break;
       }
       return *this; 
    }

    friend PlayfabServicesWildCardValue operator+(PlayfabServicesWildCardValue lhs, const PlayfabServicesWildCardValue& rhs) 
    {
       lhs += rhs; 
       return lhs; 
    }

    void AssignIfLarger(const PlayfabServicesWildCardValue& other)
    {
       if (mType != other.mType)
          return;

       switch (mType)
       {
       case ValueType::SignedInt32:
          mValue.mSignedInt = std::max<int32>(other.mValue.mSignedInt, mValue.mSignedInt);
          break;
       case ValueType::UnsignedInt32:
          mValue.mUnsignedInt += std::max<uint32>(other.mValue.mUnsignedInt, mValue.mUnsignedInt);
          break;
       case ValueType::Float:
          mValue.mFloat += std::max<float>(other.mValue.mFloat, mValue.mFloat);
          break;
       }
    }

    void AssignIfSmaller(const PlayfabServicesWildCardValue& other)
    {
       if (mType != other.mType)
          return;

       switch (mType)
       {
       case ValueType::SignedInt32:
          mValue.mSignedInt = std::min<int32>(other.mValue.mSignedInt, mValue.mSignedInt);
          break;
       case ValueType::UnsignedInt32:
          mValue.mUnsignedInt += std::min<uint32>(other.mValue.mUnsignedInt, mValue.mUnsignedInt);
          break;
       case ValueType::Float:
          mValue.mFloat += std::min<float>(other.mValue.mFloat, mValue.mFloat);
          break;
       }
    }

    bool operator==(const PlayfabServicesWildCardValue& other)
    {
       if (mType != other.mType)
          return false;

       switch (mType)
       {
       case ValueType::SignedInt32:
          return mValue.mSignedInt == other.mValue.mSignedInt;
       case ValueType::UnsignedInt32:
          return mValue.mUnsignedInt == other.mValue.mUnsignedInt;
       case ValueType::Float:
          return mValue.mFloat == other.mValue.mFloat;
       case ValueType::Boolean:
          return mValue.mBoolean == other.mValue.mBoolean;
       case ValueType::String:
          return mString == other.mString;
       case ValueType::NotSet:
          return true;
       }
       return false;
    }
    bool operator!=(const PlayfabServicesWildCardValue& other) 
    { 
       return !(*this == other); 
    }

private:
    union Value {
        bool mBoolean;
        float mFloat;
        int32_t mSignedInt;
        uint32_t mUnsignedInt;
    } mValue;

    FString mString;
    ValueType mType;
};

inline bool operator==(const PlayfabServicesWildCardValue lhs, const PlayfabServicesWildCardValue& rhs)
{
   if (lhs.getType() != rhs.getType())
      return false;

   switch (lhs.getType())
   {
   case PlayfabServicesWildCardValue::ValueType::SignedInt32:
      return lhs.asInt32() == rhs.asInt32();
   case PlayfabServicesWildCardValue::ValueType::UnsignedInt32:
      return lhs.asUInt32() == rhs.asUInt32();
   case PlayfabServicesWildCardValue::ValueType::Float:
      return lhs.asFloat() == rhs.asFloat();
   case PlayfabServicesWildCardValue::ValueType::Boolean:
      return lhs.asBoolean() == rhs.asBoolean();
   case PlayfabServicesWildCardValue::ValueType::String:
      return lhs.asString() == rhs.asString();
   case PlayfabServicesWildCardValue::ValueType::NotSet:
      return true;
   }
   return false;
}
inline bool operator!=(const PlayfabServicesWildCardValue lhs, const PlayfabServicesWildCardValue& rhs)
{
   return !(rhs == lhs);
}