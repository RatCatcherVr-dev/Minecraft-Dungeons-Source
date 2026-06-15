#pragma once
#include "TrialTypes.h"
#include "core/HttpServiceResponse.h"
#include "modules/trials/TrialsApiResponse.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTrials, Log, All);

namespace trial {
	TArray<FReplicatableTrialDef> parseDailyTrials(minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse>& response);
}
