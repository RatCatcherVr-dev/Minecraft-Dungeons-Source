#include "Dungeons.h"
#include "GrammarBuilder.h"
#include "Parser.h"

namespace parse {

//
// TokenParserBuilder
//
TokenParserBuilder& TokenParserBuilder::prefix(const PrefixParser& prefixParser) {
	parser.prefix = prefixParser;
	return *this;
}

TokenParserBuilder& TokenParserBuilder::infix(int precedence, const InfixParser& infixParser) {
	parser.infix = infixParser;
	parser.precendence = precedence;
	return *this;
}

//
// GrammarBuilder
//
namespace grammar {

TokenParserBuilder GrammarBuilder::symbol(token::Type type) {
	auto it = mParsers.find(type);
	if (it == end(mParsers)) {
		it = mParsers.emplace(type, TokenParser{}).first;
	}
	return TokenParserBuilder(it->second);
}

GrammarBuilder& GrammarBuilder::prefix(int precendence, token::Type type) {
	symbol(type).prefix([precendence](auto st) { return Expression{ st.token,{ st.parser.expression(precendence) } }; });
	return *this;
}

GrammarBuilder& GrammarBuilder::infix(int precendence, token::Type type) {
	symbol(type).infix(precendence, [precendence](auto st) { return Expression{ st.token,{ st.left, st.parser.expression(precendence) } }; });
	return *this;
}

GrammarBuilder& GrammarBuilder::postfix(int precendence, token::Type type) {
	symbol(type).infix(precendence, [precendence](auto st) { return Expression{ st.token,{ st.left } }; });
	return *this;
}

GrammarBuilder& GrammarBuilder::value(token::Type type) {
	symbol(type).prefix([](auto st) { return Expression{ st.token }; });
	return *this;
}

GrammarBuilder& GrammarBuilder::grouping(token::Type start, token::Type end, bool insertGroupNode /* = false */) {
	symbol(start).prefix([end, insertGroupNode](auto st) {
		auto expr = st.parser.expression();
		st.parser.matchOrFail(end);
		return insertGroupNode ? Expression{ st.token,{ expr } } : expr;
	});
	return *this;
}

const Grammar& defaultBooleanGrammar() {
	static auto g = GrammarBuilder()
		.value(token::Type::Value)
		.grouping(token::Type::ParenOpen, token::Type::ParenClose)
		.prefix(80, token::Type::Not)
		.infix(30, token::Type::And)
		.infix(20, token::Type::Or);
	return g;
}

const Grammar& defaultNumberGrammar() {
	static auto g = GrammarBuilder()
		.value(token::Type::Value)
		.grouping(token::Type::ParenOpen, token::Type::ParenClose)
		.prefix(80, token::Type::Not)
		.infix(70, token::Type::Asterisk)
		.infix(70, token::Type::Slash)
		.infix(60, token::Type::Plus)
		.infix(60, token::Type::Minus)
		.infix(40, token::Type::LT)
		.infix(40, token::Type::EQ)
		.infix(40, token::Type::GT)
		.infix(30, token::Type::And)
		.infix(20, token::Type::Or);
	return g;
}

}}
