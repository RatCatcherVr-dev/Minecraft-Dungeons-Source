#include "BotAutomationPCH.h"

#include "game/GameBP.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/Conversion.h"

#if !UE_BUILD_SHIPPING
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "DrawDebugHelpers.h"
#endif

APlayerAutomator::APlayerAutomator()
	: AttachedController(NULL)
	, DebugMessageIndex(2)
{
}

void APlayerAutomator::AttachController(APlayerControllerBase* ControllerToAttach)
{
	AttachedController = ControllerToAttach;
	AttachedController->InputAutomator = this;
}

void APlayerAutomator::DetachController()
{
	if (AttachedController) {
		AttachedController->InputAutomator = NULL;
		AttachedController = NULL;
	}
}

APlayerControllerBase* APlayerAutomator::GetAttachedController()
{
	return AttachedController;
}

void APlayerAutomator::StopAutomation()
{
	RemoveAction();
	DetachController();
	ClearDebugMessage(DebugMessageIndex);
}

void APlayerAutomator::BeginDestroy()
{
	StopAutomation();
	Super::BeginDestroy();
}

void APlayerAutomator::UpdateAutomation(float DeltaTime)
{
	if (AttachedController && CurrentBotAction) {
		DebugDraw();

		UpdateAutomationPaused(DeltaTime);
		if (bAutomationPaused) {
			return;
		}

		if (!CurrentBotAction->IsRunning()) {
			CurrentBotAction->StartAction();
			BroadcastActionStarted();
		}

		CurrentBotAction->UpdateAction(DeltaTime);

		AddDebugMessage(DebugMessageIndex, FString::Printf(TEXT("Bot Action: %s, Result: %s"), *CurrentBotAction->UserFriendlyName(), *CurrentBotAction->GetStatusText()));

		if (CurrentBotAction->IsFinished())
		{
			CurrentBotAction->StopAction();
			BroadcastActionFinished(CurrentBotAction->GetStatus());

			AddDebugMessage(DebugMessageIndex, FString::Printf(TEXT("Bot Action: %s, Result: %s"), *CurrentBotAction->UserFriendlyName(), *CurrentBotAction->GetStatusText()));
			RemoveAction();
		}
	}
}

void APlayerAutomator::AddAction(std::unique_ptr<PlayerBotActionBase> NewAction)
{
	RemoveAction();
	CurrentBotAction = std::move(NewAction);
	CurrentBotAction->SetPlayerAutomator(this);
}

void APlayerAutomator::RemoveAction()
{
	if (CurrentBotAction && CurrentBotAction->IsRunning())
	{
		CurrentBotAction->StopAction();
	}
	CurrentBotAction.reset();
}

void APlayerAutomator::SetAutomationPaused(bool pause)
{
	if (bAutomationPaused && !pause) {
		BotAutomation::AutomationResumedEvent.Broadcast(this);
	}
	else if (!bAutomationPaused && pause) {
		StopMovement();
		BotAutomation::AutomationPausedEvent.Broadcast(this);
	}

	bAutomationPaused = pause;
}

void APlayerAutomator::UpdateAutomationPaused(float DeltaTime)
{
	bool bShouldPause = false;

#if !UE_BUILD_SHIPPING
	const float InputTimeout = BotAutomation::CVarControllerInputTimeout.GetValueOnGameThread();
	if (InputTimeout >= 0.0f)
	{
		if (IsControllerIdle()) {
			LastControllerInputTime += DeltaTime;
		}
		else {
			LastControllerInputTime = 0.0f;
		}

		bShouldPause |= LastControllerInputTime <= InputTimeout;
	}
#endif

	SetAutomationPaused(bShouldPause);
}

void APlayerAutomator::BroadcastAutomationFinished()
{
	BotAutomation::AutomationFinishedEvent.Broadcast(this);
}

void APlayerAutomator::BroadcastActionStarted()
{
	BotAutomation::ActionStartedEvent.Broadcast(this, CurrentBotAction.get());
}

void APlayerAutomator::BroadcastActionFinished(EPlayerBotActionResult Result)
{
	BotAutomation::ActionFinishedEvent.Broadcast(this, CurrentBotAction.get(), Result);
}

void APlayerAutomator::DebugDraw()
{
#if !UE_BUILD_SHIPPING
	if (CurrentBotAction) {
		CurrentBotAction->DebugDraw();
	}
#endif
}

void APlayerAutomator::DebugDrawPathFind()
{
#if !UE_BUILD_SHIPPING
	if (ABasePlayerController* PlayerController = Cast<ABasePlayerController>(AttachedController))
	{
		const auto path = PlayerController->GetPathFollowingComponent()->GetPath();
		if (path.IsValid())
		{
			const TArray<FNavPathPoint>& NavPoints = path->GetPathPoints();

			const auto world = GetWorld();
			for (int i = 0; i < NavPoints.Num(); i++)
			{
				FVector CurrentPoint = NavPoints[i].Location;
				DrawDebugSphere(world, CurrentPoint, 25.f, 24, FColor::Magenta);
				DrawDebugLine(world, CurrentPoint, CurrentPoint + FVector(0, 0, 500), FColor::Magenta, false, -1, 0, 2.f);
				if (i > 0) {
					FVector PreviousPoint = NavPoints[i - 1].Location;
					DrawDebugLine(world, PreviousPoint, CurrentPoint, FColor::Magenta, false, -1, 0, 5.f);
				}
			}
		}
	}
#endif
}

void APlayerAutomator::AddDebugMessage(int MessageIndex, const FString& Message, float TimeToDisplay, FColor DisplayColor)
{
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage((uint64)((PTRINT)this) + MessageIndex, TimeToDisplay, DisplayColor, Message);
	}
#endif
}

void APlayerAutomator::ClearDebugMessage(int MessageIndex)
{
	AddDebugMessage(MessageIndex, TEXT(""), 0.0f);
}

bool APlayerAutomator::IsControllerIdle()
{
	const auto controller = GetAttachedController();

	const auto keyList = {
		EKeys::Gamepad_LeftX,
		EKeys::Gamepad_LeftY,
		EKeys::Gamepad_RightX,
		EKeys::Gamepad_RightY,
		EKeys::Gamepad_LeftTrigger,
		EKeys::Gamepad_RightTrigger,

		EKeys::Gamepad_FaceButton_Bottom,
		EKeys::Gamepad_FaceButton_Right,
		EKeys::Gamepad_FaceButton_Left,
		EKeys::Gamepad_FaceButton_Top,

		EKeys::Gamepad_LeftShoulder,
		EKeys::Gamepad_RightShoulder,

		EKeys::Gamepad_DPad_Up,
		EKeys::Gamepad_DPad_Down,
		EKeys::Gamepad_DPad_Right,
		EKeys::Gamepad_DPad_Left,

		EKeys::Global_Menu,
		EKeys::Global_View
	};

	for (const auto key : keyList)
	{
		const auto value = controller->PlayerInput->GetKeyValue(key);
		if (value > 0) {
			return false;
		}
	}

	return true;
}

void APlayerAutomator::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	AttachedController->InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void APlayerAutomator::InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	AttachedController->InputAxis(Key, Delta, DeltaTime, NumSamples, bGamepad);
}

void APlayerAutomator::MoveInDirection(const FVector direction)
{
	AttachedController->GetPawn()->AddMovementInput(direction);
	AttachedController->SetControlRotation(direction.ToOrientationRotator());
}

void APlayerAutomator::MoveInDirectionOf(const FVector destination)
{
	FVector playerLocation = AttachedController->GetPawn()->GetActorLocation();
	FVector direction = GetNormalizedDirection(destination, playerLocation);
	MoveInDirection(direction);
}

void APlayerAutomator::DodgeInDirection(const FVector direction)
{
	const auto Controller = Cast<ABasePlayerController>(AttachedController);
	Controller->DodgeInDirection(direction);
}

void APlayerAutomator::DodgeInDirectionOf(const FVector destination)
{
	FVector playerLocation = AttachedController->GetPawn()->GetActorLocation();
	FVector direction = GetNormalizedDirection(destination, playerLocation);
	DodgeInDirection(direction);
}

FVector APlayerAutomator::GetNormalizedDirection(const FVector target, const FVector source)
{
	FVector direction = target - source;
	direction.Normalize();
	return direction;
}

EPathFollowingRequestResult::Type APlayerAutomator::MoveToLocation(const FVector destination, const bool bUsePathFinding, const float fAcceptanceRadius, const bool bAllowPartialPath)
{
	return MoveToWrapper(destination, bUsePathFinding, fAcceptanceRadius, bAllowPartialPath, [&](ABasePlayerController* Controller) {
		const auto gameBp = GetCachedGameBP();
		const auto terrain = gameBp == nullptr ? nullptr : gameBp->GetTerrain();

		if (!GetTerrainType(terrain, destination).isReachable()) {
			return EPathFollowingRequestResult::Failed;
		}

		return Controller->MoveToLocation(destination, fAcceptanceRadius, true, bUsePathFinding, false, true, nullptr, bAllowPartialPath);
		});
}

EPathFollowingRequestResult::Type APlayerAutomator::MoveToActor(AActor* target, const bool bUsePathFinding, const float fAcceptanceRadius, const bool bAllowPartialPath)
{
	if (target->IsPendingKillOrUnreachable()) {
		return EPathFollowingRequestResult::Failed;
	}
	auto fallbackDestination = target->GetActorLocation();
	return MoveToWrapper(fallbackDestination, bUsePathFinding, fAcceptanceRadius, bAllowPartialPath, [&](ABasePlayerController* Controller) {
		return Controller->MoveToActor(target, fAcceptanceRadius, true, bUsePathFinding, true, nullptr, bAllowPartialPath);
		});
}

const float ZCheckOffset = 1000.f;

EPathFollowingRequestResult::Type APlayerAutomator::MoveToLocationStep(const FVector destination, const bool bUsePathFinding, const float fAcceptanceRadius, const bool bAllowPartialPath)
{
	ABasePlayerController* Controller = Cast<ABasePlayerController>(AttachedController);
	if (!Controller) {
		return EPathFollowingRequestResult::Failed;
	}

	StopMovement();

	const auto source = AttachedController->GetPawn()->GetActorLocation();

	const FVector above{
		destination.X,
		destination.Y,
		FMath::Max(source.Z, destination.Z) + ZCheckOffset
	};

	const auto stepSize = FMath::Sqrt(100.f * 100.f * 2.f) / 2.f; // half a diagonal sounds like a good value
	const auto stepCount = static_cast<int>(FVector::Dist2D(source, above) / stepSize);
	const auto stepCountSafe = FMath::Clamp(stepCount, 2, 10);
	const auto fractionSize = 1.f / stepCountSafe;

	const auto world = GetWorld();
	const auto gameBp = GetCachedGameBP();
	const auto terrain = gameBp == nullptr ? nullptr : gameBp->GetTerrain();

	auto moveResult = EPathFollowingRequestResult::Failed;
	for (auto fraction = 1.f - fractionSize; fraction > fractionSize || moveResult != EPathFollowingRequestResult::Failed; fraction -= fractionSize) {
		moveResult = AttemptMoveToStepLocation(source, above, fraction, terrain, world, Controller, bUsePathFinding, fAcceptanceRadius, bAllowPartialPath);
	}
	return moveResult;
}

EPathFollowingRequestResult::Type APlayerAutomator::AttemptMoveToStepLocation(FVector source, FVector above, float fFraction, Terrain* terrain, UWorld* world, ABasePlayerController* Controller, bool bUsePathFinding, float fAcceptanceRadius, bool bAllowPartialPath)
{
	const auto between = FMath::Lerp(source, above, fFraction);
	if (!GetTerrainType(terrain, between).isReachable()) {
		return EPathFollowingRequestResult::Failed;
	}

	const FVector betweenUnder{ between.X, between.Y, source.Z - ZCheckOffset };

	FHitResult hitResultBetween;
	const bool hitBelowSuccess = world->LineTraceSingleByChannel(hitResultBetween, between, betweenUnder, (ECollisionChannel)ECustomTraceChannels::IgnorePlayer);

	if (!hitBelowSuccess || IsSteep(source, hitResultBetween.Location)) {
		return EPathFollowingRequestResult::Failed;
	}

	return Controller->MoveToLocation(hitResultBetween.Location, fAcceptanceRadius, true, bUsePathFinding, false, true, nullptr, bAllowPartialPath);;
}

void APlayerAutomator::StopMovement(bool bStopPathFinding)
{
	if (AttachedController) {
		AttachedController->StopMovement();

		if (bStopPathFinding) {
			if (ABasePlayerController* PlayerController = Cast<ABasePlayerController>(AttachedController)) {
				PlayerController->AbortPathFollowing();

				const auto PathFollowingComponent = PlayerController->GetPathFollowingComponent();
				if (PathFollowingComponent && PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
				{
					PathFollowingComponent->AbortMove(*PlayerController, FPathFollowingResultFlags::ForcedScript, FAIRequestID::CurrentRequest, EPathFollowingVelocityMode::Reset);
				}
			}
		}
	}
}

terrain::Type APlayerAutomator::GetTerrainType(Terrain* terrain, const FVector& location) {
	// hack for gameBp/terrain not existing in fake maps
	if (terrain == nullptr) {
		return TerrainCell::unset;
	}

	return terrain->getType(conversion::ueToTerrain(location));
}

class AGameBP* APlayerAutomator::GetCachedGameBP()
{
	if (AttachedController) {
		if (ABasePlayerController* PlayerController = Cast<ABasePlayerController>(AttachedController)) {
			return PlayerController->GetCachedGameBP();
		}
	}
	return nullptr;
}

bool APlayerAutomator::IsTerrainReachable(const FVector location)
{
	const auto gameBp = GetCachedGameBP();
	const auto terrain = gameBp == nullptr ? nullptr : gameBp->GetTerrain();
	return GetTerrainType(terrain, location).isReachable();
}
