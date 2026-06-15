#pragma once

#include "lovika/tile/DoorDef.h"

namespace generator { namespace graph {

using Node = size_t;

struct Edge {
	struct Data { Node node; DoorDef door; };
	Data src, dst;
};

struct Graph {
	Graph(std::vector<Edge> = {});

	std::vector<Edge> edgesFrom(Node) const;
	std::vector<Edge> edgesTo(Node) const;
	const std::vector<Edge>& edges() const;
private:
	std::vector<Edge> mEdges;
};

}}
