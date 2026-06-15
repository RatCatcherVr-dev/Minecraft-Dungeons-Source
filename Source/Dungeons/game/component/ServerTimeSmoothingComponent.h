// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerTimeSmoothingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UServerTimeSmoothingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UServerTimeSmoothingComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void BeginPlay() override;

	void SetComponentTickEnabled(bool bEnabled) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetSmoothedServerTimeSeconds() const { return SmoothedServerTime; }

	//Time to smooth to the server lead time
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SmoothingTimeSeconds = 0.04f;

	//Amount of time difference from server before we start smoothing
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ToleranceStartSeconds = 0.01f;

	//Amount of time difference when we consider smoothing complete and simply set time to the desired time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ToleranceStopSeconds = 0.0005f;

private:
	float GetServerTimeSeconds() const;
	float GetLeadTimeSeconds() const;

	float SmoothedServerTime = 0;
	bool bIsSmoothing = false;
};
