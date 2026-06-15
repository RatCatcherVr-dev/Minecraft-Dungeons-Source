#pragma once

#include "CoreMinimal.h"

class TimezoneUtil final {

public:
	static FString getUTCOffset();

private:
	TimezoneUtil() = default;

	static FString getOffsetAsString(const FTimespan& utc, const FTimespan& local);
	static FString getOffsetAsString(int hoursDiff);

	const static FString DEFAULT;;
};
