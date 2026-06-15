#include "Dungeons.h"
#include "Graph.h"
#include "util/Algo.h"

namespace generator { namespace graph {

Graph::Graph(std::vector<Edge> edges /*= {}*/)
	: mEdges(std::move(edges)) {
}

std::vector<Edge> Graph::edgesTo(Node node) const {
	return algo::copy_if(mEdges, RETLAMBDA(it.dst.node == node));
}

std::vector<Edge> Graph::edgesFrom(Node node) const {
	return algo::copy_if(mEdges, RETLAMBDA(it.src.node == node));
}

const std::vector<Edge>& Graph::edges() const {
	return mEdges;
}

}}
