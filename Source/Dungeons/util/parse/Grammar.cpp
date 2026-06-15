#include "Dungeons.h"
#include "Parser.h"
#include "Grammar.h"

namespace parse {

//
// TokenParser
//
TokenParser::TokenParser()
	: precendence(0) {
}

TokenParser::TokenParser(const PrefixParser& prefix, const InfixParser& infix, int precendence)
	: prefix(prefix)
	, infix(infix)
	, precendence(precendence) {
}


//
// Grammar
//
namespace grammar {

Grammar::Grammar() {
}

Grammar::Grammar(const TokenParserMap& parsers)
	: mParsers(parsers) {
}

const PrefixParser* Grammar::prefix(token::Type type) const {
	auto it = get(type);
	return (it && it->prefix) ? &it->prefix : nullptr;
}

const InfixParser* Grammar::infix(token::Type type) const {
	auto it = get(type);
	return (it && it->infix) ? &it->infix : nullptr;
}

int Grammar::precendence(token::Type type) const {
	auto it = get(type);
	return it ? it->precendence : (type == token::Type::Eof ? -32000 : 0);
}

const TokenParser* Grammar::get(token::Type type) const {
	auto it = mParsers.find(type);
	return it != end(mParsers) ? &it->second : nullptr;
}

}}
