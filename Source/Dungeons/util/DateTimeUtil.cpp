#include "DateTimeUtil.h"

namespace DateTimeUtil
{

namespace internal
{

bool HasPassed(const FDateTime& dateTimeUTC, const FDateTime& nowUTC) {
	return nowUTC > dateTimeUTC;
}

}

bool InDateTimeWindow(const FString& windowStartUTC, const FString& windowEndUTC) {
	FDateTime windowStartTime;
	FDateTime windowEndTime;

	if (!FDateTime::ParseIso8601(*windowStartUTC, windowStartTime)) {
		check(false && "Failed to parse start date - make sure it's ISO 8601 formated");
		return false;
	}

	if (!FDateTime::ParseIso8601(*windowEndUTC, windowEndTime)) {
		check(false && "Failed to parse end date - make sure it's ISO 8601 formated");
		return false;
	}

	return InDateTimeWindow(windowStartTime, windowEndTime);
}

bool InDateTimeWindow(const FDateTime& windowStartUTC, const FDateTime& windowEndUTC) {
	const FDateTime nowUTC = FDateTime::UtcNow();
	return internal::HasPassed(windowStartUTC, nowUTC) && !internal::HasPassed(windowEndUTC, nowUTC);
}

bool HasPassed(const FString& dateTimeUTC) {
	FDateTime dateTime;

	if (!FDateTime::ParseIso8601(*dateTimeUTC, dateTime)) {
		check(false && "Failed to parse date - make sure it's ISO 8601 formated");
		return false;
	}

	return HasPassed(dateTime);
}

bool HasPassed(const FDateTime& dateTimeUTC) {
	return internal::HasPassed(dateTimeUTC, FDateTime::UtcNow());
}

FDateTime UTCToLocal(const FDateTime& dateTimeUTC) {
	const FTimespan localOffset = FDateTime::Now() - FDateTime::UtcNow();
	return dateTimeUTC + localOffset;
}

}
