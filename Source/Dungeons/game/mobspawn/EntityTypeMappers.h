#pragma once

#include "CommonTypes.h"
#include <array>
#include <functional>

enum class EntityType : unsigned int;
class Random;

namespace game {

struct FDifficulty;

namespace mobspawn {

using EntityTypeMapper = std::function<EntityType(EntityType)>;

TArray<EntityType>		AllVariants(EntityType);
	
      EntityTypeMapper  DefaultVariants(const FDifficulty&, Random* = nullptr);
const EntityTypeMapper& NoVariants();

      EntityTypeMapper  Only(EntityType);
      EntityTypeMapper  FromPredicateAndMapper(::Pred<EntityType>, EntityTypeMapper);

      EntityTypeMapper  CascadedBreakWhenTypeChanged(std::vector<EntityTypeMapper>);

}}
