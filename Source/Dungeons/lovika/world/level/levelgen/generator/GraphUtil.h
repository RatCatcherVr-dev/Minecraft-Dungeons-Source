#pragma once

#include "Graph.h"

namespace generator { namespace graph { namespace util {

struct Connection {
	Node node;
	Edge edge;

	Edge::Data& my();
	Edge::Data& other();
};

std::vector<Connection> allConnections(const Graph&, Node);

std::vector<Node> allNeighbouringConnectedNodes(const Graph&, Node);
std::vector<Node> spatiallyNeighbouringConnectedNodes(const Graph&, Node);

Edge  fakeEdge(Node src, Node dst);
Graph withEdgesAdded(const Graph&, std::vector<graph::Edge>);

}}}
