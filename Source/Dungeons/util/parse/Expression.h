#pragma once

#include "Token.h"

namespace parse {

struct Expression;

struct Expression {
	token::Token token;
	std::vector<Expression> children;
};

}
