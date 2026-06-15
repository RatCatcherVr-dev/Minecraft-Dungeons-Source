#pragma once

#include "Expression.h"
#include <functional>
#include <unordered_map>

namespace parse {
class Parser;

struct PrefixState { Parser& parser; token::Token token; };
using PrefixParser = std::function<Expression(PrefixState)>;

struct InfixState { Parser& parser; Expression left; token::Token token; };
using InfixParser = std::function<Expression(InfixState)>;

struct TokenParser {
	TokenParser();
	TokenParser(const PrefixParser&, const InfixParser&, int precendence);

	PrefixParser prefix;
	InfixParser infix;
	int precendence;
};

using TokenParserMap = std::unordered_map<token::Type, TokenParser>;

namespace grammar {

struct Grammar {
	Grammar(const TokenParserMap&);

	const PrefixParser* prefix(token::Type) const;
	const InfixParser* infix(token::Type) const;
	int precendence(token::Type) const;
protected:
	Grammar();
	const TokenParser* get(token::Type) const;

	TokenParserMap mParsers;
};

}}
