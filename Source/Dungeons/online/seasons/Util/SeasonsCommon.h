#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
#include "LogMacros.h"

#include "modules/LiveOps/LiveOpsEndpoints.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLiveOps, Log, All);

namespace online 
{
namespace liveops 
{
namespace common
{

std::string GenerateUUID(uint64 timestamp);

}

enum class UpdateRequestStatus {
	Success,
	Failure,
};

using DungeonsEndpoint = minecraft::api::endpoint::Dungeons;

FDateTime ToFDateTime(const std::string& dateTime);

}
}
