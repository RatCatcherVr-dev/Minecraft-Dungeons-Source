#pragma once

#include "CommonTypes.h"
#include "TileGroup.h"

class TileDef;

class TileSet {
public:
	void feed(Unique<TileDef>);
	void feed(std::vector<Unique<TileDef>>&);
	void feed(TileSet&&);

	TileGroup createGroup(const std::vector<io::Tile>&) const;

	bool isEmpty() const;
private:
	std::map<std::string, Unique<TileDef>> mTiles;
};
