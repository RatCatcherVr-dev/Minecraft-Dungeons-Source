#include "ClientStringUtil.h"

namespace dungeonsapiclient { namespace utils {
	FString toFString(const std::string& str) {
		return FString(str.c_str());
	}

	std::string toString(const FString& str) {
		return std::string(TCHAR_TO_UTF8(*str));
	}
}}

