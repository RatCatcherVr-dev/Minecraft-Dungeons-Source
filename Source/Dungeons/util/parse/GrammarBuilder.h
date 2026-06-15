#pragma once

#include "Grammar.h"

namespace parse {
	
struct TokenParserBuilder {
	TokenParserBuilder(TokenParser& parser) : parser(parser) {}
	TokenParserBuilder& prefix(const PrefixParser&);
	TokenParserBuilder& infix(int precedence, const InfixParser&);

	TokenParser& parser;
};
	
namespace grammar {

class GrammarBuilder : public Grammar {
public:
	TokenParserBuilder symbol(token::Type);
	GrammarBuilder& value(token::Type);
	GrammarBuilder& prefix(int precendence, token::Type);
	GrammarBuilder& infix(int precendence, token::Type);
	GrammarBuilder& postfix(int precendence, token::Type);
	GrammarBuilder& grouping(token::Type start, token::Type end, bool insertGroupNode = false);
};

const Grammar& defaultBooleanGrammar();
const Grammar& defaultNumberGrammar();

}}
