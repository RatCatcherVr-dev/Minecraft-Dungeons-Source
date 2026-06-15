

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlickeringLightComponent.h"
#include "SineFlickerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API USineFlickerComponent : public UFlickeringLightComponent
{
	GENERATED_BODY()

public:	
	
	USineFlickerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeScaleA = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeScaleB = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PowerAmplitude = 750.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PowerOffset = 7500.0f;

protected:
	
	virtual void BeginPlay() override;

private:	
	
	float TimeOffset = 0.0f;

	float Flicker();
		
};
