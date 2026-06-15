#include "Dungeons.h"
#include "Lexing.h"
#include "util/StringUtils.h"

namespace parse { namespace lex {

std::vector<std::string> splitWords(const std::string& s, std::string separators /* = "+-*!/&|()" */) {
	Util::stringReplace(Util::stringReplace(separators, " ", ""), "\t", "");

	std::vector<std::string> words;
	size_t start = 0;

	for (size_t i = 0; i < s.length(); ++i) {
		char ch = s[i];
		bool isSep = Util::stringContains(separators, ch);

		if (ch == ' ' || isSep) {
			if (i > start) {
				words.push_back(s.substr(start, i - start));
				start = i;
			}
			if (isSep) {
				words.emplace_back(1, ch);
			}
			++start;
		}
	}
	if (start < s.length()) {
		words.push_back(s.substr(start));
	}
	return words;
}

static token::Type wordToTokenType(const std::string& word) {
	if (word.length() == 1) {
		auto type = token::Type::forChar(word[0]);
		if (type != token::Type::Invalid) {
			return type;
		}
	}
	auto lower = Util::toLower(word);
	if (lower == "&&" || lower == "and" || lower == "but") {
		return token::Type::And;
	}
	if (lower == "||" || lower == "or") {
		return token::Type::Or;
	}
	if (lower == "not" || lower == "'isn't") {
		return token::Type::Not;
	}
	return token::Type::Value;
}

std::vector<token::Token> tokenize(const std::vector<std::string>& words) {
	std::vector<token::Token> tokens;

	for (auto& word : words) {
		tokens.emplace_back(wordToTokenType(word), word);
	}
	return tokens;
}

}}
