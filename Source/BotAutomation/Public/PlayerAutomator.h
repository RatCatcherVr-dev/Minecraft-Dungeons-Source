#pragma once

#include "GameFramework/PlayerController.h"
#include "Navigation/PathFollowingComponent.h"
#include "lovika/world/level/terrain/Terrain.h"

#include "PlayerAutomator.generated.h"

class AGameBP;
class APlayerControllerBase;
class ABasePlayerController;

class PlayerBotActionBase;
enum class EPlayerBotActionResult;

UCLASS()
class BOTAUTOMATION_API APlayerAutomator : public AInfo
{
	GENERATED_BODY()
public:
	APlayerAutomator();

	// Automator controller attachment management
	void AttachController(APlayerControllerBase* ControllerToAttach);
	void DetachController();
	APlayerControllerBase* GetAttachedController();
	void StopAutomation();
	virtual void BeginDestroy() override;

	// Automator action management
	void UpdateAutomation(float DeltaTime);
	void AddAction(std::unique_ptr<PlayerBotActionBase> NewAction);
	void RemoveAction();

private:
	APlayerControllerBase* AttachedController;
	std::unique_ptr<PlayerBotActionBase> CurrentBotAction;

	void OnEndPlayEvent(AActor*, EEndPlayReason::Type);

public:
	void SetAutomationPaused(bool value);
	void UpdateAutomationPaused(float DeltaTime);

	void BroadcastAutomationFinished();
	void BroadcastActionStarted();
	void BroadcastActionFinished(EPlayerBotActionResult Result);

private:
	bool bAutomationPaused;
	float LastControllerInputTime;

	// Automator debug helpers
	void DebugDraw();

	void AddDebugMessage(int MessageIndex, const FString& Message, float TimeToDisplay = 999999999.0f, FColor DisplayColor = FColor::White);
	void ClearDebugMessage(int MessageIndex);
	int32 DebugMessageIndex;

public:
	void DebugDrawPathFind();

	bool IsControllerIdle();

	// Automator input management
	void InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad);
	void InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad);
	void MoveInDirection(const FVector direction);
	void MoveInDirectionOf(const FVector destination);
	void DodgeInDirection(const FVector direction);
	void DodgeInDirectionOf(const FVector destination);
	EPathFollowingRequestResult::Type MoveToLocation(const FVector destination, const bool bUsePathFinding = true, const float fAcceptanceRadius = -1.f, const bool bAllowPartialPath = true);
	EPathFollowingRequestResult::Type MoveToActor(AActor* target, const bool bUsePathFinding = true, const float fAcceptanceRadius = -1.f, const bool bAllowPartialPath = true);
	void StopMovement(bool bStopPathFinding = true);

	bool IsTerrainReachable(const FVector location);
private:
	bool IsSteep(const FVector source, const FVector destination)
	{
		return (FMath::Square(source.Z - destination.Z) > FVector::DistSquared2D(source, destination) * 1.1f);
	}

	FVector GetNormalizedDirection(const FVector target, const FVector source);

	template<typename MoveMethod>
	EPathFollowingRequestResult::Type MoveToWrapper(const FVector destination, const bool bUsePathFinding, const float fAcceptanceRadius, const bool bAllowPartialPath, MoveMethod moveMethod)
	{
		ABasePlayerController* playerController = Cast<ABasePlayerController>(AttachedController);
		if (!playerController) {
			return EPathFollowingRequestResult::Failed;
		}

		StopMovement();

		const auto moveResult = [&]() {
			const auto source = playerController->GetPawn()->GetActorLocation();

			if (IsSteep(source, destination)) {
				return EPathFollowingRequestResult::Failed;
			}

			return moveMethod(playerController);
		}();

		if (moveResult == EPathFollowingRequestResult::Failed && bAllowPartialPath) {
			return MoveToLocationStep(destination, bUsePathFinding, fAcceptanceRadius);
		}

		return moveResult;
	}
	
	EPathFollowingRequestResult::Type MoveToLocationStep(const FVector destination, const bool bUsePathFinding = true, const float fAcceptanceRadius = -1.f, const bool bAllowPartialPath = true);
	
	EPathFollowingRequestResult::Type AttemptMoveToStepLocation(FVector source, FVector above, float fFraction, Terrain* terrain, UWorld* world, ABasePlayerController* Controller, bool bUsePathFinding, float fAcceptanceRadius, bool bAllowPartialPath);

	class AGameBP* GetCachedGameBP();
	terrain::Type GetTerrainType(Terrain* terrain, const FVector& location);
};