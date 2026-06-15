#pragma once

#include "Expression.h"
#include "eval/Boolean.h"
#include "eval/Number.h"

namespace parse {
namespace grammar {
struct Grammar;
}

TOptional<Expression> parseDefault(const std::string&, const grammar::Grammar&, const std::string& separators);
TOptional<Expression> parseDefaultBoolean(const std::string&);
TOptional<Expression> parseDefaultNumber(const std::string&);

template <typename T>
TOptional<eval::boolean::PredicateNode<T>> predicateDefault(const std::string& text, const eval::boolean::PredicateFactory<T>& factory) {
	if (auto expr = parseDefaultBoolean(text)) {
		return eval::boolean::toPredicateNodes(expr.GetValue(), factory);
	}
	return {};
}

template <typename T>
TOptional<eval::number::NumberNode<T>> numberDefault(const std::string& text, const eval::number::ProviderFactory<T>& factory) {
	if (auto expr = parseDefaultNumber(text)) {
		return eval::number::toNumberNodes(expr.GetValue(), factory);
	}
	return {};
}

}
