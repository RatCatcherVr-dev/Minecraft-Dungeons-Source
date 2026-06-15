#pragma once
#include "CoreMinimal.h"
#include "ReusedEffectActor.generated.h"

UCLASS()
class DUNGEONS_API AReusedEffectActor : public AActor {
	GENERATED_BODY()
public:
	void ResetEffect(float newLifeSpan);
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnEffectReset();
};

