#pragma once
#include "DateTime.h"

class FString;

namespace DateTimeUtil
{

bool InDateTimeWindow(const FString& windowStartUTC, const FString& windowEndUTC);
bool InDateTimeWindow(const FDateTime& windowStartUTC, const FDateTime& windowEndUTC);

bool HasPassed(const FString& dateTimeUTC);
bool HasPassed(const FDateTime& dateTimeUTC);

FDateTime UTCToLocal(const FDateTime& dateTimeUTC);

}
