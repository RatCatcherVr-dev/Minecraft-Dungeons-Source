#pragma once

#include "util/parse/Expression.h"
#include "util/parse/Token.h"
#include <vector>

namespace parse { namespace eval {
	
template <typename T>
T mapTree(const Expression& tree) {
	std::vector<T> children;
	for (auto&& child : tree.children) {
		children.emplace_back(mapTree<T>(child));
	}
	return T{ tree.token, children };
}

}}
