#include "Dungeons.h"
#include "ParserFactory.h"
#include "GrammarBuilder.h"
#include "Lexing.h"
#include "Parser.h"

namespace parse {

TOptional<parse::Expression> parseDefault(const std::string& text, const grammar::Grammar& grammar, const std::string& separators) {
	auto words = lex::splitWords(text, separators);
	auto tokens = lex::tokenize(words);
	if (auto result = Parser(grammar).parse(tokens)) {
		return result.expr;
	}
	return {};
}

TOptional<parse::Expression> parseDefaultBoolean(const std::string& text) {
	return parseDefault(text, grammar::defaultBooleanGrammar(), "+-*!/&|()");
}

TOptional<parse::Expression> parseDefaultNumber(const std::string& text) {
	return parseDefault(text, grammar::defaultNumberGrammar(), "()");
}

}
