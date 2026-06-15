#include "Dungeons.h"
#include "TileDebug.h"

namespace Util {

	TileDebugData traverse(const game::Tiles& tiles, std::unordered_set<std::string> visited, const game::Tile& tile) {
		const auto& id = tile.tile().id();

		std::vector<TileDebugData> children;

		for (auto&& neighbor : tiles.getAllNeighbours(tile)) {
			const auto& neighborId = neighbor->tile().id();

			const auto query = visited.find(neighborId);
			if (query == visited.end()) {
				visited.insert(neighborId);
				children.push_back(traverse(tiles, visited, *neighbor));
			}
		}

		std::sort(
			children.begin(),
			children.end(),
			[](const auto& a, const auto& b) { return a.children.size() < b.children.size(); }
		);

		return { id, tile.tilePlacement().bounds(), std::move(children) };
	}

	TileDebugData collectTiles(const game::Tiles& tiles) {
		std::unordered_set<std::string> visited;

		visited.insert(tiles.getTiles().front()->tile().id());

		return traverse(tiles, visited, *tiles.getTiles().front());
	}

}
