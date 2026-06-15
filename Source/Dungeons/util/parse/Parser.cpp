#include "Dungeons.h"
#include "Parser.h"

namespace parse {

static const Expression fail = { token::Token(token::Type::Invalid, "") };

Parser::Parser(const grammar::Grammar& grammar)
	: mGrammar(grammar)
	, mError(Error::Success) {
}

Result Parser::parse(const std::vector<token::Token>& tokens) {
	mTokens = tokens;
	mIndex = mErrorIndex = -1;
	mError = Error::Success;
	auto expr = expression(-1);
	return{ expr, mError, mErrorIndex };
}

Expression Parser::expression(int precedence /* = 0 */) {
	if (mError != Error::Success) {
		return fail;
	}

	auto token = readToken();
	auto prefix = mGrammar.prefix(token.type);
	if (!prefix) {
		return failWith(Error::Fail);
	}
	Expression expr = (*prefix)({ *this, token });

	while (precedence < mGrammar.precendence(peekToken().type)) {
		auto lastToken = token;
		token = readToken();
		auto infix = mGrammar.infix(token.type);
		if (!infix) {
			return failWith(Error::Fail);
		}
		expr = (*infix)({ *this, expr, token });
	}
	return expr;
}

bool Parser::match(token::Type type) {
	if (type != peekToken().type) {
		return false;
	}
	readToken();
	return true;
}

bool Parser::matchOrFail(token::Type type) {
	if (!match(type)) {
		failWith(Error::Fail);
		return false;
	}
	return true;
}

const token::Token& Parser::readToken() {
	if (mIndex >= static_cast<int>(mTokens.size()) - 1) {
		return token::Token::Eof;
	}
	return mTokens[++mIndex];
}

const token::Token& Parser::peekToken() const {
	if (mIndex >= static_cast<int>(mTokens.size()) - 1) {
		return token::Token::Eof;
	}
	return mTokens[mIndex + 1];
}

Expression Parser::failWith(Error error) {
	if (mError == Success) {
		mErrorIndex = mIndex;
		mError = error;
	}
	return fail;
}

}
