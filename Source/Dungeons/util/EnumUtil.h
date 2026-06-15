#pragma once

#include "EnumUtil.hpp"
#include "TypeTraits.h"

template<typename EnumType>
FORCEINLINE FString GetEnumValueToString(EnumType value) {
	//static_assert(has_value<enum_name_tag<EnumType>>::value, "Enum type is missing name tag. Insert ENUM_NAME(EnumType); under your enum type declaration");
	return internal::GetEnumValueToString(enum_name_tag<EnumType>::value, value);
}

template<typename EnumType>
FORCEINLINE FString GetEnumValueToStringStripped(EnumType value) {
	//static_assert(has_value<enum_name_tag<EnumType>>::value, "Enum type is missing name tag. Insert ENUM_NAME(EnumType); under your enum type declaration");
	return internal::GetEnumValueToStringStripped(enum_name_tag<EnumType>::value, value);
}

template <typename EnumType>
FORCEINLINE TOptional<EnumType> GetEnumValueFromStringT(const FString& s) {
	return internal::GetEnumValueFromString<EnumType>(enum_name_tag<EnumType>::value, s);
}

//D11.PS - Console fix - use FString instead of the literal directly
//#define EnumValueFromString(EnumType, name) internal::GetEnumValueFromString<EnumType>(L#EnumType, name)
//#define EnumValueFromString(EnumType, name) internal::GetEnumValueFromString<EnumType>(TCHAR_TO_ANSI(#EnumType), name)
#define EnumValueFromString(EnumType, name) internal::GetEnumValueFromString<EnumType>(*FString(#EnumType), name)

namespace enumUtil {
	template < typename C, C beginVal, C endVal>
	class Iterator {
		typedef typename std::underlying_type<C>::type val_t;
		int val;
	public:
		Iterator(const C& f) : val(static_cast<val_t>(f)) {}
		Iterator() : val(static_cast<val_t>(beginVal)) {}
		Iterator operator++() {
			++val;
			return *this;
		}
		C operator*() { return static_cast<C>(val); }
		Iterator begin() { return *this; } //default ctor is good
		Iterator end() {
			static const Iterator endIter = ++Iterator(endVal); // cache it
			return endIter;
		}
		bool operator!=(const Iterator& i) { return val != i.val; }
	};
}
