#pragma once

#include "game/util/ConsoleCommandHelpers.h"
#include "util/StringUtil.h"
#include "world/entity/EntityTypes.h"

namespace affector { namespace parse {


template <typename T> struct Type {
	TOptional<T> parse(const FString& s) const { return GetEnumValueFromStringT<T>(s); }
};

template <> struct Type<FString> {
	TOptional<FString> parse(const FString& s) const { return s; }
};

template <> struct Type<EntityType> {
	TOptional<EntityType> parse(const FString& s) const {
		return MaybeEntityTypeFromString(stringutil::toStdString(s.TrimStartAndEnd()));
	}
};

template <typename T>
MaybeTypeWeight<T> typeWeight(const FAffectorData& data) {
	const FString s = data.AsString().TrimStartAndEnd();

	if (const auto onlyProbability = ArgAsFloat(s)) {
		return TypeWeight<T>{ {}, onlyProbability.GetValue() };
	}
	if (const auto onlyType = Type<T>{}.parse(s)) {
		return TypeWeight<T>{ onlyType, 1 };
	}

	FString typeString, probString;
	if (s.Split(":", &typeString, &probString)) {
		if (const auto probability = ArgAsFloat(probString)) {
			if (const auto target = Type<T>{}.parse(typeString)) {
				return TypeWeight<T>{ target, probability.GetValue() };
			}
		}
	}
	return {};
}

template <typename Src, typename Dst>
TOptional<std::pair<MaybeTypeWeight<Src>, MaybeTypeWeight<Dst>>> typeWeightPair(const FAffectorData& data) {
	FString src, dst;

	if (!data.AsString().Split("->", &src, &dst)) {
		return {};
	}
	return std::make_pair(typeWeight<Src>(src), typeWeight<Dst>(dst));
}

}}
