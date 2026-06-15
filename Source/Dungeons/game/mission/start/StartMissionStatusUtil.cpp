#include "Dungeons.h"
#include "StartMissionStatusUtil.h"

bool UStartMissionStatusUtil::IsStartable(const FStartMissionStatus& status) {
	return !status.mProblem.IsSet() && status.mSuccess.IsSet();
}

bool UStartMissionStatusUtil::HasProblem(const FStartMissionStatus& status) {
	return status.mProblem.IsSet();
}

FProblemStatus UStartMissionStatusUtil::GetProblem(const FStartMissionStatus& status) {
	return status.mProblem.Get({});
}

bool UStartMissionStatusUtil::HasSuccess(const FStartMissionStatus& status) {
	return status.mSuccess.IsSet();
}

FSuccessStatus UStartMissionStatusUtil::GetSuccess(const FStartMissionStatus& status) {
	return status.mSuccess.Get({});
}
