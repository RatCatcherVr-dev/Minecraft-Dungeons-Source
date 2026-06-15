#include "Dungeons.h"
#include "Token.h"

namespace parse { namespace token {

Type::Type(unsigned char id)
	: mId(id)
	, mChar(0) {
}

Type::Type(unsigned char id, char ch)
	: mId(id)
	, mChar(ch) {
}

const Type
Type::Invalid       (0),
Type::Plus          (10, '+'),
Type::Minus         (11, '-'),
Type::Asterisk      (20, '*'),
Type::Slash         (21, '/'),
Type::And           (30, '&'),
Type::Or            (31, '|'),
Type::Not           (32, '!'),
Type::ParenOpen     (40, '('),
Type::ParenClose    (41, ')'),
Type::LT            (60, '<'),
Type::EQ            (61, '='),
Type::GT            (62, '>'),
Type::Value         (90),
Type::Eof           (255);

unsigned char Type::id() const {
	return mId;
}

bool Type::hasChar() const {
	return mChar != 0;
}

char Type::getChar() const {
	return mChar;
}

const std::vector<Type> Type::types = { {
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
	Invalid
} };

Type Type::forChar(char ch) {
	if (ch != 0) {
		for (auto&& type : types) {
			if (ch == type.getChar()) {
				return type;
			}
		}
	}
	return Invalid;
}

bool Type::operator==(const Type& rhs) const {
	return mId == rhs.mId;
}

bool Type::operator!=(const Type& rhs) const {
	return mId != rhs.mId;
}

const Token Token::Eof(Type::Eof, "");

}}
