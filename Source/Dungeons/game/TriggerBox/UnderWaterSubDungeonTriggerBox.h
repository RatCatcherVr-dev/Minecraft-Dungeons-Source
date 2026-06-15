#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameplayEffectTypes.h"
#include "UnderWaterSubDungeonTriggerBox.generated.h"
/**
 *
 */
UCLASS()
class DUNGEONS_API AUnderWaterSubDungeonTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	void NotifyActorBeginOverlap(AActor* OtherActor) override;


	void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	TMap<AActor* , TArray<FActiveGameplayEffectHandle>> ActiveGameplayEffectsHandles;
};