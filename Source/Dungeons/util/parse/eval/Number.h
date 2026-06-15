#pragma once

#include "util/parse/Expression.h"
#include "util/parse/Token.h"
#include "EvalUtil.h"
#include "CommonTypes.h"
#include "util/Algo.h"

namespace parse { namespace eval { namespace number {

using Number = double;

template <typename T>
using Provider = std::function<Number(T)>;

template <typename T>
using ProviderFactory = std::function<Provider<T>(const token::Token&)>;


template <typename T>
struct NumberNode {
	token::Token token;
	std::vector<NumberNode<T>> children;
	Provider<T> provider;

	Number eval(const T& value) const {
		if (provider) {
			return provider(value);
		}
		if (token::Type::Plus == token.type) {
			return algo::sum(children, RETLAMBDA(it.eval(value)));
		}
		if (token::Type::Minus == token.type) {
			return children[0].eval(value) - children[1].eval(value);
		}
		if (token::Type::Asterisk == token.type) {
			return children[0].eval(value) * children[1].eval(value);
		}
		if (token::Type::Slash == token.type) {
			return children[0].eval(value) / children[1].eval(value);
		}
		if (token::Type::GT == token.type) {
			return children[0].eval(value) > children[1].eval(value);
		}
		if (token::Type::EQ == token.type) {
			return children[0].eval(value) == children[1].eval(value);
		}
		if (token::Type::LT == token.type) {
			return children[0].eval(value) < children[1].eval(value);
		}
		if (token::Type::Not == token.type) {
			return !children[0].eval(value);
		}
		if (token::Type::And == token.type) {
			return algo::all_of(children, RETLAMBDA(it.eval(value)));
		}
		if (token::Type::Or == token.type) {
			return algo::any_of(children, RETLAMBDA(it.eval(value)));
		}
		return false;
	}
};

inline TOptional<double> asDouble(const std::string& s) {
	char* p;
	auto number = strtod(s.c_str(), &p);
	return (p != s.c_str()) ? number : TOptional<double>{};
}

template <typename T>
void populate(NumberNode<T>& tree, const ProviderFactory<T>& valueFactory) {
	if (auto asNumber = asDouble(tree.token.data)) {
		tree.provider = [number = asNumber.GetValue()](const T&) { return number; };
	} else {
		tree.provider = valueFactory(tree.token);
	}
	for (auto&& child : tree.children) {
		populate(child, valueFactory);
	}
}

template <typename T>
NumberNode<T> toNumberNodes(const Expression& tree, const ProviderFactory<T>& valueFactory) {
	auto root = mapTree<NumberNode<T>>(tree);
	populate(root, valueFactory);
	return root;
}

}}}
