


#include "FlickeringLightComponent.h"



UFlickeringLightComponent::UFlickeringLightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 2.0f;

}

void UFlickeringLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFlickeringLightComponent::SetIntensityCoarse(float newIntensity)
{
	if (mIntensityFraction < 1.0f)
	{
		newIntensity = FMath::Lerp(newIntensity * .2f, newIntensity, mIntensityFraction);
		const float FlickerCoursenessCheck = FMath::Lerp(FlickerCoarseness * .2f, FlickerCoarseness, mIntensityFraction);

		if (mCachedLight && FMath::Abs(newIntensity - mCachedLight->Intensity) >= FlickerCoursenessCheck)
		{
			mCachedLight->SetIntensity(newIntensity);
		}
	}
	else
	{
		if (mCachedLight && FMath::Abs(newIntensity - mCachedLight->Intensity) >= FlickerCoarseness)
		{
			mCachedLight->SetIntensity(newIntensity);
		}
	}
	
}

void UFlickeringLightComponent::BeginPlay()
{
	Super::BeginPlay();

	mCachedLight = Cast<ULightComponent>(GetOwner()->FindComponentByClass<ULightComponent>());
}


