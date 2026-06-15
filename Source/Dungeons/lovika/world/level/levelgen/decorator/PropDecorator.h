#pragma once

#include "Decorator.h"
#include "lovika/world/level/levelgen/TileGroup.h"

class TileSet;

namespace decorator {

class PropDecorator: public TileDecorator {
public:
	PropDecorator(TileGroup);

	void decorate(State) const override;
private:
	TileGroup mProps;
};
}
