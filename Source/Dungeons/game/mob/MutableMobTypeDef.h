#pragma once

#include "MobTypeDef.h"

namespace mob { namespace type {
	struct MutableMobTypeDef : public MobTypeDef {
		using MobTypeDef::MobTypeDef;
		MutableMobTypeDef& name(FText txt) { mName = txt; return *this; }
		MutableMobTypeDef& description(FText txt) { mDescriptionText = txt; return *this; }
		MutableMobTypeDef& disabled() { bDisabled = true; return *this; }
		MutableMobTypeDef& iconRowName(const FName& name) { mIconRowNameProvider = [name]() { return name; }; return *this; }
		MutableMobTypeDef& iconRowName(EntityType entityType) { mIconRowNameProvider = [entityType]() { return EntityTypeToFName(entityType); }; return *this; }
	};
}}
