


#include "TimelineFlickerComponent.h"
#include "Engine/World.h"
#include "Curves/CurveFloat.h"

// Sets default values for this component's properties
UTimelineFlickerComponent::UTimelineFlickerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;
}



void UTimelineFlickerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	mCachedTimeline = GetOwner()->FindComponentByClass<UTimelineComponent>();
	if (mCachedTimeline)
	{
		TSet<UCurveBase*> curves;
		mCachedTimeline->GetAllCurves(curves);
		TArray<UCurveBase*> curvesArray = curves.Array();
		mCachedFloatCurve = Cast<UCurveFloat>(curvesArray[0]);
		mCachedFloatCurve->GetTimeRange(mMinTime, mMaxTime);
	}
}



void UTimelineFlickerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (mCachedLight && mCachedLight->IsVisible() && mCachedTimeline)
	{
		if (mCachedTimeline->IsPlaying() && mCachedFloatCurve)
		{
			const float duration = mMaxTime - mMinTime;
			const float wrappedTime = mMinTime + FMath::Fmod(GetWorld()->GetTimeSeconds(), duration);

			SetIntensityCoarse(mCachedFloatCurve->GetFloatValue(wrappedTime));
		}
		
	}
}

