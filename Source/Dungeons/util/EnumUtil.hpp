#pragma once

namespace internal {

template<typename EnumType>
FORCEINLINE FString GetEnumValueToString(const TCHAR* enumName, EnumType value) {
	if (const UEnum* enumClass = FindObject<UEnum>(ANY_PACKAGE, enumName, true)) {
		return enumClass->GetNameByValue((int64)value).ToString();
	}
	return "EInvalid::Invalid";
}

template<typename EnumType>
FORCEINLINE FString GetEnumValueToStringStripped(const TCHAR* enumName, EnumType value) {
	const auto s = GetEnumValueToString(enumName, value);
	int index;
	return s.FindLastChar(':', index) ? s.RightChop(index + 1) : "Invalid";
}

template <typename EnumType>
static FORCEINLINE TOptional<EnumType> GetEnumValueFromString(const TCHAR* enumName, const FString& s) {
	if (const UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, enumName, true)) {
		auto value = Enum->GetValueByName(FName(*s));
		return value != INDEX_NONE ? static_cast<EnumType>(value) : TOptional<EnumType>{};
	} else {
		ensure(false && "Could not find UEnum*");
		return {};
	}
}

}
