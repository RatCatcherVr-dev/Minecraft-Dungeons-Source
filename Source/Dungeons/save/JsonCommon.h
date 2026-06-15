#pragma once

#include <string>
#include "util/StringUtil.h"
#include "util/EnumUtil.h"
#include "writer.h"

namespace json { namespace common {

template <typename EnumType>
std::string enumString(EnumType enumValue) {
	return stringutil::toStdString(GetEnumValueToStringStripped(enumValue));
}

template <typename EnumType>
TOptional<EnumType> stringEnum(std::string string) {
	return GetEnumValueFromStringT<EnumType>(stringutil::toFString(string));
}

template <typename KeyType, typename ValueType, typename KeyCreator, typename ValueCreator>
TMap<KeyType, ValueType> JsonMapToTMap(const Json::Value& node, const KeyCreator& keyCreator, const ValueCreator& valueCreator) {
	TMap<KeyType, ValueType> map;
	for (auto&& name : node.getMemberNames()) {
		map.Add(keyCreator(name), valueCreator(node[name]));
	}
	return map;
}

template <typename Iterable, typename KeyCreator, typename ValueCreator>
Json::Value TMapToJsonMap(const Iterable& iterable, const KeyCreator& keyCreator, const ValueCreator& valueCreator) {
	Json::Value map(Json::objectValue);
	for (auto&& pair : iterable) {
		std::string key = keyCreator(pair.Key);
		TOptional<Json::Value> value = static_cast<Json::Value>(valueCreator(pair.Value));
		if (value.IsSet()) {
			map[std::move(key)] = std::move(value.GetValue());
		}
	}
	return map;
}

template <typename Iterable, typename Creator>
Json::Value iterableToJsonArray(const Iterable& iterable, const Creator& creator) {
	Json::Value items(Json::arrayValue);
	for (auto&& item : iterable) {
		TOptional<Json::Value> value = static_cast<Json::Value>(creator(item));
		if (value.IsSet()) {
			items.append(std::move(value.GetValue()));
		}
	}
	return items;
}

template <typename Iterable>
Json::Value iterableToJsonArray(const Iterable& iterable) {
	return iterableToJsonArray(iterable, RETLAMBDA(stringutil::toStdString(it)));
}

template <typename ValueType>
Json::Value toJson(const std::unordered_map<std::string, ValueType>& nameMap) {
	Json::Value node;
	for (const auto& namePair : nameMap) {
		node[namePair.first] = namePair.second;
	}
	return node;
}

}}