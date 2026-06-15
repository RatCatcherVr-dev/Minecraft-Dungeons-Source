#pragma once

#include "CoreMinimal.h"
#include "HardwareSurvey/Public/HardwareSurvey.h"
#include "Containers/Ticker.h"


class APlayerControllerBase;
struct FHardwareSurveyResults;

class DUNGEONS_API AutoDetect
{
public:
	AutoDetect();

	void StartAutoDetect(APlayerControllerBase* PlayerControllerBase);

protected:
	bool AutoDetectCanRun();
	bool IsHardwareSurveyRequired();
	bool TickHardwareSurvey(float delta);
	void OnHardwareSurveyCompleted(const FHardwareSurveyResults& HWSResults);

private:
	bool mPendingHardwareSurveyResults;

	FDelegateHandle TickerHandle;
	APlayerControllerBase* mPlayerControllerBase;
};
