#pragma once

#include "CoreMinimal.h"
#include <string>

namespace dungeonsapiclient { namespace utils {
	DUNGEONSCLIENTUTILS_API FString toFString(const std::string&);
	DUNGEONSCLIENTUTILS_API std::string toString(const FString&);
}}
