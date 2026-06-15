#pragma once

#include "CoreMinimal.h"
#include "core/Types.h"
#include "core/JsonSerializer.h"

namespace minecraft {
namespace api {

inline bool lowerEquals(const std::string& a, const std::string& b) {
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
		return tolower(a) == tolower(b);
	});
}

class LiveOpsJsonParseException : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

inline void throwAndlogError(const std::string& error, const std::string& key) {
	auto msg = error + ": '" + key + "'.";
	UE_LOG(LogTemp, Error, TEXT("%s"), *FString(msg.c_str()));
	ensure(false && "Perhaps the seasons api has changed, see log for the unexpected key");
	throw LiveOpsJsonParseException(msg);
}

template<typename T>
TOptional<T> parseOptional(const Json::Value& jsonValue, const std::string& key) {
	if (!jsonValue.isMember(key)) {
		return  {};
	}
	return *parseJson<T>(jsonValue, key);
}

template<typename T>
T parseRequired(const Json::Value& jsonValue, const std::string& key) {
	const auto optional = parseOptional<T>(jsonValue, key);
	if (!optional.IsSet()) {
		throwAndlogError("A required seasons field is missing from the json", key);
	}
	return optional.GetValue();
}

template<typename T>
TOptional<std::vector<T>> parseOptionalArray(const Json::Value& jsonValue, const std::string& key) {
	if (!jsonValue.isMember(key)) {
		return  {};
	}

	if (!jsonValue[key].isArray()) {
		throwAndlogError("Expected array value", key);
	}

	return *parseArrayJson<T>(jsonValue, key);
}

template<typename T>
std::vector<T> parseRequiredArray(const Json::Value& jsonValue, const std::string& key) {
	const auto optional = parseOptionalArray<T>(jsonValue, key);
	if (!optional.IsSet()) {
		throwAndlogError("A required seasons array field is missing from the json", key);
	}
	return optional.GetValue();
}

template<typename EnumType>
TOptional<EnumType> parseOptionalEnum(const Json::Value& jsonValue, const std::string& key, std::function<TOptional<EnumType>(const std::string&)> stringToEnum) {
	const auto optionalStr = parseOptional<std::string>(jsonValue, key);
	if (!optionalStr.IsSet()) {
		return {};
	}
	return stringToEnum(optionalStr.GetValue());
}

template<typename EnumType>
EnumType parseRequiredEnum(const Json::Value& jsonValue, const std::string& key, std::function<TOptional<EnumType>(const std::string&)> stringToEnum) {
	const auto optional = parseOptionalEnum<EnumType>(jsonValue, key, stringToEnum);
	if (!optional.IsSet()) {
		throwAndlogError("A required seasons enum field is missing from the json", key);
	}
	return optional.GetValue();
}

}
}
