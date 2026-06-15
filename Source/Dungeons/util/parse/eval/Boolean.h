#pragma once

#include "util/parse/Expression.h"
#include "util/parse/Token.h"
#include "EvalUtil.h"
#include "CommonTypes.h"

namespace parse { namespace eval { namespace boolean {

template <typename T>
using PredicateFactory = std::function< ::Pred<T>(const token::Token&)>;

template <typename T>
::Pred<T> TrueFactory() {
	return [](const T& t) { return true; };
}

template <typename T>
::Pred<T> FalseFactory() {
	return [](const T& t) { return false; };
}

template <typename T>
struct PredicateNode {
	token::Token token;
	std::vector<PredicateNode<T>> children;
	::Pred<T> predicate;

	bool eval(const T& value) const {
		if (predicate) {
			return predicate(value);
		}
		if (token::Type::Not == token.type) {
			return !children[0].eval(value);
		}
		if (token::Type::And == token.type) {
			return std::find_if_not(begin(children), end(children), [value](const auto& pred) { return pred.eval(value); }) == end(children);
		}
		if (token::Type::Or == token.type) {
			return std::find_if(begin(children), end(children), [value](const auto& pred) { return pred.eval(value); }) != end(children);
		}
		return false;
	}
};

template <typename T>
void populatePredicates(PredicateNode<T>& tree, const PredicateFactory<T>& valueFactory) {
	if (tree.token.data == "true") {
		tree.predicate = TrueFactory<T>();
	} else if (tree.token.data == "false") {
		tree.predicate = FalseFactory<T>();
	} else {
		tree.predicate = valueFactory(tree.token);
	}
	for (auto&& child : tree.children) {
		populatePredicates(child, valueFactory);
	}
}

template <typename T>
PredicateNode<T> toPredicateNodes(const Expression& tree, const PredicateFactory<T>& valueFactory) {
	auto root = mapTree<PredicateNode<T>>(tree);
	populatePredicates(root, valueFactory);
	return root;
}

}}}
