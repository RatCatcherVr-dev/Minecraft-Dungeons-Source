#include "SeasonsCommon.h"
#include "Util/StringUtil.h"

namespace online 
{
namespace liveops 
{
namespace common
{

std::string GenerateUUID(uint64 timestamp) {
	/* UUID version 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                          time_low                             |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |       time_mid                |         time_hi_and_version   |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	auto time_low = (unsigned long)(timestamp & 0xFFFFFFFF);
    auto time_mid = (unsigned short)((timestamp >> 32) & 0xFFFF);
	auto time_hi_and_version = (unsigned short)((timestamp >> 48) & 0x0FFF);
	time_hi_and_version |= (1 << 12);

	auto uuid = FGuid::NewGuid();
	uint32_t A = 0; uint32_t B = 0; uint32_t C = 0; uint32_t D = 0;
	A = time_low;
	B = (time_mid << 16) | time_hi_and_version;
	C = uuid.C;
	D = uuid.D;

	uuid = FGuid(A, B, C, D);
	auto output = std::string(TCHAR_TO_UTF8(*uuid.ToString()));
	output.insert(8, "-");
	output.insert(13, "-");
	output.insert(18, "-");
	output.insert(23, "-");
	UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] UUID before format: %s"), *uuid.ToString());
	UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] UUID after format: %s"), UTF8_TO_TCHAR(output.c_str()));
	return output;
}

}

FDateTime ToFDateTime(const std::string& dateTime) {
	FDateTime result(0);

	ensureAlwaysMsgf(FDateTime::ParseIso8601(*stringutil::toFString(dateTime), result),
		TEXT("Failed to parse date time string: %s"), UTF8_TO_TCHAR(dateTime.c_str()));

	return result;
}

}
}
