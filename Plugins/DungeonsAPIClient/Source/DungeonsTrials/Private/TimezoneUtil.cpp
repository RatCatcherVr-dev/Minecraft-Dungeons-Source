#include "TimezoneUtil.h"

const FString TimezoneUtil::DEFAULT = "0000";

FString TimezoneUtil::getUTCOffset() {
	const auto utc = FTimespan::FromSeconds(FDateTime::UtcNow().ToUnixTimestamp());
	const auto local = FTimespan::FromSeconds(FDateTime::Now().ToUnixTimestamp());
	return getOffsetAsString(utc, local);
}

FString TimezoneUtil::getOffsetAsString(const FTimespan& utc, const FTimespan& local) {
	const auto localHours = local.GetTotalHours();
	const auto utcHours = utc.GetTotalHours();	
	const auto hoursDiff = localHours - utcHours;
	return getOffsetAsString(static_cast<int>(round(hoursDiff)));
}

FString TimezoneUtil::getOffsetAsString(int hoursDiff) {
	if (hoursDiff > 14 || hoursDiff < -12) {
		return DEFAULT;
	}

	FString offset;

	if (hoursDiff < 0 ) {
		offset += "-";
		hoursDiff *= -1;
	}

	if (hoursDiff >= 0 && hoursDiff < 10) {
		offset += "0";
	}

	offset += FString::FromInt(hoursDiff);
	offset += "00";

	return offset;
}
