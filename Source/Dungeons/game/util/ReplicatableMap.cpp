#include "Dungeons.h"
#include "ReplicatableMap.h"
#include "util/Algo.hpp"
#include "util/StringUtil.h"

FPair::FPair(FString key, FString value)
	: key(std::move(key))
	, value(std::move(value)) {
}

FPair::FPair(TPair<FString, FString> pair)
	: key(pair.Key)
	, value(pair.Value) {
}

FReplicatableMap::FReplicatableMap(TMap<FString, FString>& sourceMap) : pairs(MakeReplicatable(sourceMap)) {
}

void FReplicatableMap::Add(const FString& key, const FString& value) {
	pairs.Add(FPair(key, value));
}

void FReplicatableMap::Add(const std::string& key, const std::string& value) {
	Add(stringutil::toFString(key), stringutil::toFString(value));
}

TOptional<FString> FReplicatableMap::Find(const FString& key) const {
	for (const auto& pair : pairs) {
		if (pair.key == key) {
			return pair.value;
		}
	}
	return TOptional<FString>();
}

TMap<FString, FString> FReplicatableMap::ToTMap() const {
	TMap<FString, FString> newMap;
	for (const auto& pair : pairs) {
		newMap.Add(pair.key, pair.value);
	}
	return newMap;
}

TArray<FPair> FReplicatableMap::MakeReplicatable(TMap<FString, FString>& sourceMap) {
	TArray<FPair> pairs;
	for (const auto& pair : sourceMap) {
		pairs.Add(FPair(pair));
	}
	return pairs;
}
