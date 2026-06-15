#pragma once
 
#include "MobTypeDef.h"
#include "world/entity/EntityTypes.h"

namespace mob { namespace type {
const MobTypeDef& get(EntityType);
const MobTypeDef* getChecked(EntityType);
const FText& getMobDisplayName(EntityType);
const TArray<EntityType> getAllEnabled();
}}

