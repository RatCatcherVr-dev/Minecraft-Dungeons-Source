#pragma once

#include "LevelVariationType.h"
#include "LevelVariationDef.h"
#include "MutableLevelVariationDef.h"
#include "game/defs/DefsContainer.h"

namespace missions { namespace variation {
	using LevelVariationDefs = DefsContainer<MutableLevelVariationDef, LevelVariationDef, ELevelVariationType>;
	const LevelVariationDefs& defs();
}}