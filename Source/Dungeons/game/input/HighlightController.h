#pragma once

#include "InputController.h"
#include "HighlightController.generated.h"

class UTargetingTickStageComponent;
class ABasePlayerController;
class APlayerCharacter;
class ACharacter;
class UInteractableComponent;


struct HighlightedActorInfo {
	HighlightedActorInfo(AActor* actor) :Actor(actor), interactableComponent(nullptr){
	}
	TWeakObjectPtr<AActor> Actor;
	
	TOptional<InputController::EOutlineState> outline;
	TOptional<InputController::ESilhouetteState> silhouette;

	mutable uint8 currentState = 0;

	int outlineCount = 0;

	bool sticky = false;

	TWeakObjectPtr<UInteractableComponent> interactableComponent;

	bool operator==(const AActor* other) const {
		return Actor == other;
	}
};

UCLASS()
class DUNGEONS_API AHighlightController : public AActor {
	GENERATED_BODY()

public:
	AHighlightController(const FObjectInitializer& ObjectInitializer);
	
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	static AHighlightController* CheckHighlightControllerExists(UWorld* pWorld);	
	void BeginSilhouetteActor(AActor* actor);
	void EndSilhouetteActor(AActor* actor);

	/** return true if this actor was not previously highlighted. */
	void BeginOutlineActor(APlayerCharacter* playerCharacter, AActor* actor, UInteractableComponent* interactableComponent = nullptr, bool sticky = false);
	/** return true if this actor is still highlighted. */
	void EndOutlineActor(AActor* actor);

	void Enable(bool enabled);
	void Claim(ABasePlayerController* controller, UTargetingTickStageComponent* ticker);

	void Tick(float DeltaTime) override;
	
	FORCEINLINE bool IsActorHighlighted(AActor* actor) const {
		if (HighlightedActors.Contains(actor)) {
			const auto state = HighlightedActors.FindByKey(actor);
			return state->outlineCount > 0;
		}
		return false;
	}

private:
	void HighlighObscuredCharacters(const FVector& sourcePoint, const FVector& cameraLocation);
	void UpdateHighlightedActorInfo();
	void ApplyHighlightedActorInfo(const HighlightedActorInfo& info, uint8 newState);

	void ClearAllHighlights();

	bool PlayerObscured = false;
	float MainPlayerOcclusionSilhouettingTimer = 0;
	TArray<HighlightedActorInfo> HighlightedActors;
	TArray<TWeakObjectPtr<AActor>> ActorsToCheck;

	bool bDisableHighlightsOneFrame = false;

	void UpdateOcclusionSilhouetting();

	HighlightedActorInfo& GetOrCreateForActor(AActor* actor);

	UPROPERTY(transient)
	ABasePlayerController* PlayerController = nullptr;

	UPROPERTY(transient)
	UTargetingTickStageComponent* Ticker = nullptr;

	bool IsLocalCoop = false;
public:
	void DisableHighlightsForOneFrame();
};