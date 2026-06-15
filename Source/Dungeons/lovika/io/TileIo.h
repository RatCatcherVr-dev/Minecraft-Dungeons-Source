#pragma once

#include "lovika/io/ObjectGroupFile.h"
#include "lovika/tile/TilePlacement.h"

class TileDef;

namespace io { namespace tile {

struct Objects {
	std::vector<Unique<TileDef>> tiles;
	std::vector<Unique<TileDef>> props;
};

Objects fromObjectGroup(const io::ObjectGroup&);

}}
