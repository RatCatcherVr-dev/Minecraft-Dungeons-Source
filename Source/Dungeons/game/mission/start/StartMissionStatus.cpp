#include "Dungeons.h"
#include "StartMissionStatus.h"

FStartMissionStatus FStartMissionStatus::fail(const FProblemStatus& Problem) {
	return {
		{},
		Problem,		
	};
};

FStartMissionStatus FStartMissionStatus::success(const FSuccessStatus& Success) {
	return {
		Success,
		{},
	};
};
