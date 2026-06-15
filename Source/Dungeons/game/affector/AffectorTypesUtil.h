#pragma once

#include <Optional.h>
#include "AffectorData.h"

enum class EntityType : uint32;
enum class EEnchantmentTypeID : uint8;

struct FAffectorData;

namespace affector { namespace parse {

template <class T>
struct TypeWeight {
	TOptional<T> type;
	float weight = 1;
};

template <class T>
using MaybeTypeWeight = TOptional<TypeWeight<T>>;

// Parses a single 'Type:Number' string into TypeWeight instance where
// Type or :Number are optional. Type is optionally type converted. Examples:
//  - "Type:0.3" returns {Type, 0.3}
//  - "Type"     returns {Type, 1  }
//  - "0.4"      returns {{},   0.4}
template <typename T = FString>
MaybeTypeWeight<T> typeWeight(const FAffectorData&);

// Parses a pair of TypeWeights with an arrow between. Example: "Src:0.5 -> Dst"
template <typename Src, typename Dst>
TOptional<std::pair<MaybeTypeWeight<Src>, MaybeTypeWeight<Dst>>> typeWeightPair(const FAffectorData&);

template <typename T>
struct TargetAndProbability {
	using TargetType = T;

	T target;
	float probability = 1;
};

using ReplaceMobTypeInfo = TargetAndProbability<EntityType>;
using MobEnchantmentInfo = TargetAndProbability<EEnchantmentTypeID>;

TOptional<ReplaceMobTypeInfo> replaceMobTypeInfo(const FAffectorData&);
TOptional<MobEnchantmentInfo> mobEnchantmentInfo(const FAffectorData&);

}}

#include "AffectorTypesUtil.hpp"
