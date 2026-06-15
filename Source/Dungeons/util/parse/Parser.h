#pragma once

#include "Expression.h"
#include "Grammar.h"
#include <vector>

namespace parse {
struct Result;

class Parser {
public:
	enum Error { Success, Fail };
	Parser(const grammar::Grammar&);

	Result parse(const std::vector<token::Token>&);
	Expression expression(int precedence = 0);
	bool match(token::Type);
	bool matchOrFail(token::Type);
private:
	const token::Token& readToken();
	const token::Token& peekToken() const;

	Expression failWith(Error);

	const grammar::Grammar& mGrammar;
	std::vector<token::Token> mTokens;
	int mIndex;
	int mErrorIndex;
	Error mError;
};

struct Result {
	Expression expr;
	Parser::Error error;
	int mErrorIndex;

	bool success() const {
		return error == Parser::Success;
	}
	operator void*() const { // (if (result) ...
		return (void*) (success() ? this : nullptr);
	}
};

}
