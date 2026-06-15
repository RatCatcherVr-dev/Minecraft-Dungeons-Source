#include "Dungeons.h"
#include "GraphUtil.h"
#include "Graph.h"
#include "util/Algo.h"

namespace generator { namespace graph { namespace util {

Edge::Data& Connection::my() { return node == edge.dst.node ? edge.src : edge.dst; }
Edge::Data& Connection::other() { return node == edge.dst.node ? edge.dst : edge.src; }

std::vector<Connection> allConnections(const Graph& graph, Node node) {
	std::vector<Connection> out;

	const auto addAll = [&](const std::vector<Edge>& edges) {
		std::transform(edges.begin(), edges.end(), std::back_inserter(out), RETLAMBDA((Connection{ it.src.node == node ? it.dst.node : it.src.node, it })));
	};
	addAll(graph.edgesFrom(node));
	addAll(graph.edgesTo(node));
	return out;
}

std::vector<Node> allNeighbouringConnectedNodes(const Graph& graph, Node node) {
	std::vector<Node> nodes;
	for (auto& connection : allConnections(graph, node)) {
		algo::add_unique(nodes, connection.node);
	}
	return nodes;
}

std::vector<Node> spatiallyNeighbouringConnectedNodes(const Graph& graph, Node node) {
	std::vector<Node> nodes;
	for (auto& connection : allConnections(graph, node)) {
		if (connection.edge.src.door.isSpatiallyAdjacent(connection.edge.dst.door)) {
			algo::add_unique(nodes, connection.node);
		}
	}
	return nodes;
}

Edge fakeEdge(Node src, Node dst) {
	static const DoorDef FakeDoor(BlockPos::ZERO, Facing::UP, 1, "", "");
	return Edge{ {src, FakeDoor}, {dst, FakeDoor} };
}

Graph withEdgesAdded(const Graph& graph, std::vector<graph::Edge> added) {
	auto edges = graph.edges();
	edges.insert(edges.end(), std::make_move_iterator(added.begin()), std::make_move_iterator(added.end()));
	return Graph(edges);
}

}}}
