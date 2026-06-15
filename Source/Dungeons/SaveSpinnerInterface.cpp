

#include "SaveSpinnerInterface.h"
#include "Engine/Engine.h"


static double sSaveSpinnerStartTime = -1.0;
static double sSaveSpinnerMinTime = -1.0;

static volatile bool bIsSpinnerVisible = false;


//d11.gm - allows blueprints to get the spinner visiblity 
bool USaveSpinnerInterface::GetIsSpinnerVisible()
{
	if (sSaveSpinnerMinTime < 0.0) //no spinner
	{
		return false;
	}

	if (sSaveSpinnerStartTime > 0.0)
	{
		//we have been set spinning
		const double SpinningTime = FPlatformTime::Seconds() - sSaveSpinnerStartTime;

		if (SpinningTime >= sSaveSpinnerMinTime)
		{
			//trc compliant
			sSaveSpinnerStartTime = -1.0;
			
		}

		return true;
	}

	return bIsSpinnerVisible;
}

void USaveSpinnerInterface::SetIsSpinnerVisible(bool visible)
{
	if (visible && sSaveSpinnerMinTime >= 0.0)
	{
		sSaveSpinnerStartTime = FPlatformTime::Seconds();
	}

	bIsSpinnerVisible = visible;
}

void USaveSpinnerInterface::InitialiseSaveSpinner()
{
	if (!GConfig) {
		return;
	}

	FString SectionName = "/Script/Dungeons.SaveIndicator";

	bool DisplaySaveIndicator;
	GConfig->GetBool(*SectionName, TEXT("DisplaySaveIndicator"), DisplaySaveIndicator, GGameIni);

	if (DisplaySaveIndicator)
	{
		float MinimumDisplayTime;
		GConfig->GetFloat(*SectionName, TEXT("MinimumDisplayTime"), MinimumDisplayTime, GGameIni);
		sSaveSpinnerMinTime = (double)MinimumDisplayTime;
	}
	
}
