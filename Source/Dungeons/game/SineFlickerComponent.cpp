


#include "SineFlickerComponent.h"
#include "Engine/World.h"

USineFlickerComponent::USineFlickerComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	
	PrimaryComponentTick.TickInterval = 0.5f;
}



void USineFlickerComponent::BeginPlay()
{
	Super::BeginPlay();

	TimeOffset = FMath::RandRange(0.0f, 100.0f);

}


// Called every frame
void USineFlickerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (mCachedLight)
	{
		SetIntensityCoarse(Flicker());
	}
	
}


float USineFlickerComponent::Flicker()
{

	const float t = GetWorld()->GetTimeSeconds() + TimeOffset;

	const float result = FMath::Sin(t * TimeScaleA) + FMath::Sin(t * TimeScaleA * TimeScaleB);

	return (result * PowerAmplitude) + PowerOffset;
}
