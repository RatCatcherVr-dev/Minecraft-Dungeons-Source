

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "EffectGroupParams.h"
#include "EffectActor.h"
#include "EffectGroup.generated.h"

UCLASS()
class DUNGEONS_API UEffectGroup : public UObject {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SpawnEffect(TSubclassOf<class AReusedEffectActor> actorClass, const FTransform& transform);

	UFUNCTION(BlueprintCallable)
	void SetParams(FEffectGroupParams params);
private:
	void DrawDebugVisuals();

	UPROPERTY(EditDefaultsOnly)
	FEffectGroupParams Parameters;

	UPROPERTY()
	TArray<FEffectActor> SpawnedActors;
};

