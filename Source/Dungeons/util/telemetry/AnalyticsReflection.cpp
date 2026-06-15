#include "Dungeons.h"
#include "AnalyticsReflection.h"
#include "util/EnumUtil.h"

namespace analytics
{
	FString GetTelemetryEventTypeAsString(ETelemetryEventType EnumValue, bool StripClassName /*= false*/) {
		return StripClassName ? GetEnumValueToStringStripped(EnumValue) : GetEnumValueToString(EnumValue);
	}
}
