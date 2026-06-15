#pragma once

#include "CoreMinimal.h"
#include "CommonTypes.h"
#include "Components/ActorComponent.h"
#include "UnrealRespawnCandidateComponent.generated.h"

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API URespawnCandidateComponent : public UActorComponent {
	GENERATED_BODY()
public:
	URespawnCandidateComponent();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void SetIsRespawnActive(bool active) { isRespawnActive = active; }

	UFUNCTION(BlueprintCallable)
	const bool GetIsRespawnActive() { return isRespawnActive; }

	FVector GetRespawnLocation() { return GetOwner()->GetActorLocation(); }

	// #D11.CM - Used for exceptions where an unreal actor is used as decoration, ideally I would suggest making a barebones version of the actor that doesn't have this component.
	UPROPERTY(EditAnywhere)
	bool RemoveOnStart = false;

private:
	bool isRespawnActive = true;
};