#pragma once

#include "Token.h"
#include <vector>

namespace parse { namespace lex {

std::vector<std::string> splitWords(const std::string& s, std::string separators = "+-*!/&|()");

std::vector<token::Token> tokenize(const std::vector<std::string>& words);

}}
