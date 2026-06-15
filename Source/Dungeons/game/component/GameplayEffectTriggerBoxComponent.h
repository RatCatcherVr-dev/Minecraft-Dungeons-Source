#pragma once

#include <CoreMinimal.h>
#include <Components/BoxComponent.h>
#include <GameplayEffect.h>
#include "GameplayEffectTriggerBoxComponent.generated.h"

class AActor;
class UPrimitiveComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UGameplayEffectTriggerBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
public:
	UGameplayEffectTriggerBoxComponent();

	void BeginPlay();
	//void EndPlay(EEndPlayReason::Type EndPlayReason); // @todo: if this is ever on an actor with limited lifetime, we need to remove the applied effects when removed

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> Effects;
private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
