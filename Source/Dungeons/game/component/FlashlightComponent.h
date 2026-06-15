#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/Dimmer.h"
#include "FlashlightComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlashlightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	float increment = 1.5f;
	float decrement = 3.6f;

private:
	void UpdateLight();

	TWeakObjectPtr<ADimmer> dimmer;

	TWeakObjectPtr<ULightComponent> flashlight;
	float initialIntensity;
	float initialTemperature;

	float fraction;
};
