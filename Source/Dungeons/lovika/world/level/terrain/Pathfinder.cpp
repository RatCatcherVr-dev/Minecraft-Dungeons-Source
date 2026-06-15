#include "Dungeons.h"
#include "Pathfinder.h"

namespace pathfinder {
	std::vector<TerrainPos> trace(const Terrain& terrain, TerrainPos start, TerrainPos end) {
		struct Cell {			
			TerrainPos parent;
			int score;
		};

		struct Diagonal {
			TerrainPos offset;
			std::array<TerrainPos, 2> dependancies;
		};

		std::queue<TerrainPos> queue { { start } };
		std::unordered_map<TerrainPos, Cell> visited;

		const auto exploreNeighbor = [&](TerrainPos current, TerrainPos offset, int score) {
			const auto neighbor = current + offset;

			if (terrain.getType(neighbor).isReachable()) {
				const auto maybeVisited = visited.find(neighbor);

				if (maybeVisited == visited.end()) {
					visited[neighbor] = { current, score };
					queue.push(neighbor);
				} else if (maybeVisited->second.score > score) {
					visited[neighbor] = { current, score };
				}
			}
		};

		visited[start] = { start, 0 };

		while (!queue.empty()) {
			const auto current = queue.front();
			queue.pop();

			if (current == end) {
				break;
			}

			const auto currentScore = visited[current].score;
						
			std::array<TerrainPos, 4> offsets{ {
				{  0,  1 },
				{  1,  0 },
				{  0, -1 },
				{ -1,  0 },
			} };

			for (const auto offset : offsets) {
				exploreNeighbor(current, offset, currentScore + 70);
			}
			
			std::array<Diagonal, 4> diagonals { {
				Diagonal { {  1,  1 }, { { {  1, 0 }, { 0,  1 } } } },
				Diagonal { { -1,  1 }, { { { -1, 0 }, { 0,  1 } } } },
				Diagonal { {  1, -1 }, { { {  1, 0 }, { 0, -1 } } } },
				Diagonal { { -1, -1 }, { { { -1, 0 }, { 0, -1 } } } }
			} };

			for (const auto diagonal : diagonals) {
				if (
					terrain.getType(current + diagonal.dependancies[0]).isReachable() ||
					terrain.getType(current + diagonal.dependancies[1]).isReachable()
				) {
					exploreNeighbor(current, diagonal.offset, currentScore + 99);
				}
			}
		}

		std::vector<TerrainPos> path;

		auto current = end;
		while (current != start) {
			path.push_back(current);
			current = visited[current].parent;
		}

		return path;
	}
}
