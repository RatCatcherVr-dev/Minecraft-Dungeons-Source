

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"

#include "FlickeringLightComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UFlickeringLightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UFlickeringLightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetIntensityCoarse(float newIntensity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FlickerCoarseness = 0.5f;

	void SetIntensityFraction(float fIntensityFraction) {	mIntensityFraction = fIntensityFraction;}

protected:

	UPROPERTY()
	ULightComponent* mCachedLight = nullptr;

	float mIntensityFraction = 1.0f;

	virtual void BeginPlay() override;
		
};
