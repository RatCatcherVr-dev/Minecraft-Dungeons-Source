#include "Dungeons.h"
#include "GeneratorGraphUtil.h"
#include "Graph.h"
#include "Generator.h"
#include "GeneratorUtil.h"
#include "util/Algo.h"

namespace generator { namespace graph { namespace util {

Graph createGraph(const std::vector<Tile>& tiles) {
	std::vector<Edge> edges;
	for (size_t i = 0; i < tiles.size(); ++i) {
		for (auto& doorPair : tiles[i].placeResult.doorPairs) {
			auto srcTileIndex = generator::util::findIntersectingIndex(tiles, doorPair.from.position());
			checkf(srcTileIndex, TEXT("Door center needs to intersect a tile!"));
			edges.push_back(Edge{ {srcTileIndex.GetValue(), doorPair.from}, {i, doorPair.to} });
		}
	}
	return { edges };
}

int createStrayPathProgress(const Graph& graph, const std::vector<generator::Tile>& tiles, int tileIndex, std::vector<levelgen::TileProgress>& progresses, int runningSubPathIndex) {
	std::unordered_map<size_t, size_t> strayPathTiles;

	const std::function<void(size_t, size_t)> spread = [&](size_t node, size_t strayPathIndex) {
		if (progresses[node].globalTileIndex < 0 && strayPathTiles.count(node) == 0) { // index < 0 means not initalized (and by that, not on main path)
			strayPathTiles.insert(std::make_pair(node, strayPathIndex));
			algo::for_each(graph.edgesFrom(node), [&](const auto& it) { spread(it.dst.node, strayPathIndex + 1); });
		}
	};
	for (auto& edge : graph.edgesFrom(tileIndex)) {
		strayPathTiles.clear();
		spread(edge.dst.node, 1);
		if (strayPathTiles.empty()) {
			continue;
		}
		const size_t strayPathLength = algo::max_element_by(strayPathTiles, RETLAMBDA(it.second))->second + 1;
		const levelgen::TileProgress originatingTileProgress = progresses[tileIndex];

		for (const auto& nodeDistancePair : strayPathTiles) {
			levelgen::TileProgress& progress = progresses[nodeDistancePair.first];
			progress = originatingTileProgress;
			progress.stretchIndex = tiles[nodeDistancePair.first].stretchId.index; // Keep the previously assigned stretch id
			progress.strayPathSubId = runningSubPathIndex;
			progress.strayPathIndex = nodeDistancePair.second;
			progress.strayPathLength = strayPathLength;
		}
		runningSubPathIndex++;
	}
	return runningSubPathIndex;
}


std::vector<levelgen::TileProgress> createProgress(const Graph& graph, const std::vector<generator::Tile>& tiles) {
	std::vector<levelgen::TileProgress> progresses(tiles.size(), levelgen::TileProgress{ -1,-1,-1,-1,-1,-1 });

	const auto mainPathIndices = findMainPathTileIndices(graph, tiles, findEndTileIndex(tiles));

	int stretchIndex = -1;
	int stretchTileIndex = 0;

	// First, update all tiles on main path...
	for (size_t k = 0; k < mainPathIndices.size(); ++k) {
		const size_t tileIndex = mainPathIndices[k];
		const generator::Tile& tile = tiles[tileIndex];

		// If new stretch, set local stretch tile index to 0
		if (stretchIndex != tile.stretchId.index) {
			stretchIndex = tile.stretchId.index;
			stretchTileIndex = 0;
		}
		progresses[tileIndex] = levelgen::TileProgress{ stretchIndex, stretchTileIndex, static_cast<int>(k), -1, 0, 0 };
		stretchTileIndex++;
	}
	// ...then all tiles originating from main path
	int runningSubPathIndex = 0;
	for (size_t tileIndex : mainPathIndices) {
		runningSubPathIndex = createStrayPathProgress(graph, tiles, tileIndex, progresses, runningSubPathIndex);
	}
	return progresses;
}

int findEndTileIndex(const std::vector<generator::Tile>& tiles) {
	if (const auto metadataEndTileIndex = algo::index_of_if(tiles, RETLAMBDA(it.metaTile.metadata.isGoal))) {
		return metadataEndTileIndex.GetValue();
	}

	std::pair<int, int> highestTileStretchIndex{ -1, -1 };
	for (size_t i = 0; i < tiles.size(); ++i) {
		if (tiles[i].stretchId.isDefinitelyOffMainPath) {
			continue;
		}
		const int stretch = tiles[i].stretchId.mainPathIndex;
		if (stretch >= highestTileStretchIndex.second) {
			highestTileStretchIndex = { static_cast<int>(i), stretch };
		}
	}
	return highestTileStretchIndex.first;
}


std::vector<size_t> findMainPathTileIndices(const Graph& graph, const std::vector<Tile>& tiles, size_t goalTileIndex) {
	std::vector<size_t> mainPath, currentlyVisited;

	const std::function<void(size_t)> dfs = [&](size_t node) {
		currentlyVisited.push_back(node);

		if (node == goalTileIndex) {
			for (auto& visited : currentlyVisited) {
				algo::add_unique(mainPath, visited);
			}
		}
		for (auto& edge : graph.edgesFrom(node)) {
			if (edge.dst.node != node && !algo::contains(currentlyVisited, edge.dst.node)) {
				dfs(edge.dst.node);
			}
		}
		currentlyVisited.erase(currentlyVisited.end() - 1);
	};
	dfs(0);
	return mainPath;
}

}}}
