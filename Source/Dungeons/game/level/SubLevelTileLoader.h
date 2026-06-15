#pragma once

#include "SubLevelLoader.h"

class TilePlacement;

namespace game { namespace sublevel {

class TileLoader: public Loader {
public:
	using Loader::Loader;

	ULevelStreamingDynamic* schedule(const FString& path, const TilePlacement&);
};

}}
