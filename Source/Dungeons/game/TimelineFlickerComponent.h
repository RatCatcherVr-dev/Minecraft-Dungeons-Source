

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlickeringLightComponent.h"
#include "Components/TimelineComponent.h"
#include "TimelineFlickerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UTimelineFlickerComponent : public UFlickeringLightComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimelineFlickerComponent();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY()
	UTimelineComponent* mCachedTimeline;

	UPROPERTY()
	UCurveFloat* mCachedFloatCurve = nullptr;

	float mMinTime = 0.0f;
	float mMaxTime = 0.0f;
public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
