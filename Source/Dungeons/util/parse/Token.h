#pragma once

#include <string>

namespace parse { namespace token {

struct Type {
	static const Type
		Plus,
		Minus,
		Asterisk,
		Slash,
		And,
		Or,
		Not,
		ParenOpen,
		ParenClose,
		Value,

		LT,
		EQ,
		GT,

		Eof,
		Invalid;

	unsigned char id() const;
	bool hasChar() const;
	char getChar() const;

	bool operator==(const Type&) const;
	bool operator!=(const Type&) const;

	static const std::vector<Type> types;
	static Type forChar(char ch);
private:
	Type(unsigned char id);
	Type(unsigned char id, char);

	unsigned char mId;
	char mChar;
};


struct Token {
	static const Token Eof;

	Token(Type type, const std::string& data)
	:	type(type)
	,	data(data) {
	}

	Type type;
	std::string data;
};

}}

namespace std {
	template<>
	struct hash<parse::token::Type> {
		size_t operator()(const parse::token::Type& keyval) const {
			return keyval.id();
		}
	};
}
