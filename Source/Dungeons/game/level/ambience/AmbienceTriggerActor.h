#pragma once

#include "GameFramework/Actor.h"
#include "AmbienceTriggerActor.generated.h"

UCLASS()
class DUNGEONS_API AAmbienceTriggerActor : public AActor {
	GENERATED_BODY()
public:
	///Executed at runtime
	UFUNCTION(BlueprintImplementableEvent)
	void OnAmbienceActivated(const TArray< AActor* >& ActivatedAmbienceActors);

	///Executed at runtime
	UFUNCTION(BlueprintImplementableEvent)
	void OnAmbienceDeactivated(const TArray< AActor* >& DeactivatedAmbienceActors);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnAmbienceShow();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnAmbienceHide();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerExitedAmbience(APlayerCharacter* player);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerEnteredAmbience(APlayerCharacter* player);
};
