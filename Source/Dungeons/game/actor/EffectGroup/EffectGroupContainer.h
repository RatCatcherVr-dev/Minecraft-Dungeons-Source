#pragma once

#include "CoreMinimal.h"
#include "EffectGroupParams.h"
#include "EffectGroupContainer.generated.h"

class AReusedEffectActor;
class UEffectGroup;

UCLASS()
class AEffectsGroupContainerActor : public AInfo {
	GENERATED_BODY()
public:
	AEffectsGroupContainerActor() {};

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SpawnEffectInGroup(TSubclassOf<AReusedEffectActor> actorClass, const FTransform& transform, FEffectGroupParams parameters);
private:
	UPROPERTY(Transient)
	TMap<UClass*, UEffectGroup*> Groups;
};

UCLASS()
class UEffectGroupContainer : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "worldContext"))
	static void SpawnEffectInGroup(UObject* worldContext, TSubclassOf<AReusedEffectActor> actorClass, const FTransform& transform, FEffectGroupParams parameters);
};
