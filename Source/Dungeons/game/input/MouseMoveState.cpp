#include "Dungeons.h"
#include "MouseMoveState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/util/ActorQuery.h"
#include "util/EnumUtil.h"
#include <Kismet/GameplayStatics.h>
#include "game/Conversion.h"
#include "game/component/movement/MovementFlyingCommon.h"
#include "MouseButtonElytraDiveState.h"
#include "game/GameBP.h"


TAutoConsoleVariable<int32> CVarDebugPathHeuristics(
	TEXT("Dungeons.DebugDraw.PathHeuristics"),
	0,
	TEXT("Enables debug drawing of path heuristics.\n")
	TEXT("<= 0: off.\n")
	TEXT(">  0: on.\n"),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> CVarAvoidKillzones(
	TEXT("Dungeons.Navigation.AvoidKillzones"),
	1,
	TEXT("Enables killzone avoiding helpers.\n")
	TEXT("<= 0: off.\n")
	TEXT(">  0: on.\n"),
	ECVF_Cheat
);

namespace input {

	MouseInputStateTypeId MouseMoveState::OnEnterState(
		ABasePlayerController& controller,
		TargetController& targetController,
		const IMouseInputState& previousState,
		const SMouseState& mouseState)
	{
		Gamepad = mouseState.MoveAxis.Active;
		if( Gamepad )
		{
			return UndefinedState;
		}

		controller.AbortPathFollowing();
		
		PathfindOnEnd = true;
		targetController.SetTargetMode(TargetController::ETargetMode::Sticky);

		FHitResult hitResult;
		controller.GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::IgnorePlayer), false, hitResult);
		controller.OnLocationClicked(hitResult.Location);

		return UndefinedState;
	}

	void MouseMoveState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		targetController.SetTargetMode(TargetController::ETargetMode::Default);
	}

	input::MouseInputStateTypeId MouseMoveState::OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		return UndefinedState;
	}

	terrain::Type MouseMoveState::GetTerrainType(Terrain* terrain, const FVector& location) {
		// hack for gameBp/terrain not existing in fake maps
		if (terrain == nullptr) {
			return TerrainCell::unset;
		}

		return terrain->getType(conversion::ueToTerrain(location));
	}

	bool isLava(uint8_t id) {
		static const auto stillLavaId = Block::mStillLava->getId();
		static const auto flowingLavaId = Block::mFlowingLava->getId();

		return id == stillLavaId ||
			id == flowingLavaId;
	}

	bool MouseMoveState::IsDirectSafe(Terrain* terrain, FVector from, FVector to) {
		// hack for gameBp/terrain not existing in fake maps
		if (terrain == nullptr) {
			return false;
		}

		const int stepCount = FMath::Abs(from.X - to.X) + FMath::Abs(from.Y - to.Y) + 1;
		const auto step = (to - from) / stepCount;
		
		FVector cursor = from;
		for (auto i = 0; i < stepCount; i++, cursor += step) {
			const auto midpoint = conversion::ueToTerrain(cursor);
			
			if (
				terrain->getType(midpoint).isKillzone() ||
				isLava(terrain->getBlock(midpoint))
			) {
				return false;
			}
		}

		return true;
	}

	bool MouseMoveState::IsComplexPath(const TArray<FNavPathPoint>& points) {
		// complexity can be number of nodes or non-linearity (sum over path angles?)
		if (points.Num() < 3) {
			return false;
		}

		if (points.Num() > 7) {
			return true;
		}
		
		{
			// sum over all path segments
			float sum { 0.f };
			for (auto i = 1; i < points.Num(); i++) {
				sum += FVector::Dist2D(points[i - 1], points[i]);
			}

			if (sum > 3000.f) {
				return true;
			}
		}

		{
			// sum over all "angles"
			float sum { 0.f };
			for (auto i = 1; i < points.Num() - 1; i++) {
				const FVector2D delta0 { points[i].Location - points[i - 1].Location };
				const FVector2D delta1 { points[i + 1].Location - points[i].Location };

				sum += 1.f - FVector2D::DotProduct(delta0.GetSafeNormal(), delta1.GetSafeNormal());

				if (sum > 2.f) {
					return true;
				}
			}
		}

		return false;
	}

	void MouseMoveState::PathTowardsCursor(ABasePlayerController& controller, bool usePathfinding) {
		const auto world = controller.GetWorld();
		const auto gameBp = controller.GetCachedGameBP();

		const auto movement = controller.GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
		if (movement->IsFalling()) {
			controller.StopMovement();
			return;
		}

		// hack for gameBp not existing in fake maps
		const auto terrain = gameBp == nullptr ? nullptr : gameBp->GetTerrain();

		FHitResult hitResult;
		
		const ECustomTraceChannels channel = usePathfinding ? ECustomTraceChannels::IgnorePlayer : ECustomTraceChannels::PlayerPlane;
		const bool hitSuccess = controller.GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)channel), false, hitResult);

		const auto hitLocation = [&] {
			if (usePathfinding) {
				return hitResult.Location;
			}

			const auto above = hitResult.Location + FVector { 0.f, 0.f, 2000.f };
			const auto below = hitResult.Location - FVector { 0.f, 0.f, 2000.f };

			FHitResult hitResultApprox;
			const bool hitSuccessApprox = world->LineTraceSingleByChannel(hitResultApprox, above, below, (ECollisionChannel)ECustomTraceChannels::IgnorePlayer);

			if (CVarDebugPathHeuristics.GetValueOnGameThread()) {
				DrawDebugLine(world, above, below, FColor::Yellow, true, 3.f);
			}

			return hitSuccessApprox ? hitResultApprox.Location : hitResult.Location;
		}();

		const auto source = controller.GetPawn()->GetActorLocation();

		const bool initialFailure = [&]() {
			if (!hitSuccess) {
				return true;
			}

			{
				auto sourceZ = source.Z;

				if (const auto* capsuleComponent = controller.GetPawn()->FindComponentByClass<UCapsuleComponent>()) {
					sourceZ -= capsuleComponent->GetScaledCapsuleHalfHeight();
				}

				const auto distanceZ = FMath::Abs(sourceZ - hitLocation.Z);
				const auto distanceXY = FVector::Dist2D(source, hitLocation);

				if (distanceZ > distanceXY + 100.f) {
					return true;
				}
			}
			
			if (!GetTerrainType(terrain, hitLocation).isReachable()) {
				return true;
			}
			
			const auto result = controller.MoveToLocation(hitLocation, -1.f, true, usePathfinding);

			if (usePathfinding) {
				const auto pathFollowingComponent = controller.FindComponentByClass<UPlayerPathFollowingComponent>();
				if (const auto path = pathFollowingComponent->GetPath()) {
					if (IsComplexPath(path->GetPathPoints()) && IsDirectSafe(terrain, source, hitLocation)) {
						// revert to simple path finding if original path is too complex
						usePathfinding = false;
						return true;
					}
				}
			}

			return result == EPathFollowingRequestResult::Failed;
		}();
				
		if (initialFailure) {
			const FVector above {
				hitLocation.X,
				hitLocation.Y,
				FMath::Max(source.Z, hitLocation.Z) + 1000.f
			};

			if (CVarDebugPathHeuristics.GetValueOnGameThread()) {
				DrawDebugLine(world, source, above, FColor::White, true, 3.f);
			}

			bool success = false;
						
			const auto stepSize = FMath::Sqrt(100.f * 100.f * 2.f) / 2.f; // half a diagonal sounds like a good value
			const auto stepCount = static_cast<int>(FVector::Dist2D(source, above) / stepSize);
			const auto stepCountSafe = FMath::Clamp(stepCount, 2, 10);
			const auto fractionSize = 1.f / stepCountSafe;

			for (auto fraction = 1.f - fractionSize; fraction > fractionSize; fraction -= fractionSize) {
				const auto between = FMath::Lerp(source, above, fraction);
				
				if (!GetTerrainType(terrain, between).isReachable()) {
					continue;
				}

				const FVector betweenUnder { between.X, between.Y, source.Z - 1000.f };

				FHitResult hitResultBetween;
				const bool hitBelowSuccess = world->LineTraceSingleByChannel(hitResultBetween, between, betweenUnder, (ECollisionChannel)ECustomTraceChannels::IgnorePlayer);

				if (
					!hitBelowSuccess ||
					(FMath::Square(source.Z - hitResultBetween.Location.Z) > FVector::DistSquared2D(source, hitResultBetween.Location) * 1.1f)
				) {
					continue;
				}

				if (CVarDebugPathHeuristics.GetValueOnGameThread()) {
					DrawDebugLine(world, between, betweenUnder, FColor::White, true, 3.f);
				}

				auto resultBetween = controller.MoveToLocation(hitResultBetween.Location, -1.f, true, usePathfinding);
				if (resultBetween != EPathFollowingRequestResult::Failed) {
					success = true;
					break;
				}
			}

			if (!success) {
				controller.StopMovement();
			}
		}
	}

	MouseInputStateTypeId MouseMoveState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (!mouseState.MoveButton.Pressed) {
			//So as it turns out, this happens after component tick, but before state machine tick. To not end up having a frame of no movement input
			// we just do another move here. Dirty, I know...but I rather not poke around ordering of things in the input state machine at this stage.
			FVector direction =  GetMouseProjection(controller) - controller.GetPawn()->GetActorLocation();
			direction.Normalize();
			MoveInDirection(controller, direction);
			PathTowardsCursor(controller, PathfindOnEnd);
			return GetInterruptState(controller, targetController, mouseState);
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseMoveState::OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (mouseState.RangedButton.Pressed) return MouseRangedState::TryEnterState(controller, mouseState);

		return UndefinedState;
	}

	EMouseCursorStates MouseMoveState::GetCursorState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		if (mouseState.MoveButton.Pressed || Gamepad) {
			return EMouseCursorStates::MoveHeld;
		}
		return EMouseCursorStates::UNSET;
	}

	MouseInputStateTypeId MouseMoveState::GetInterruptState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		return GetId<MouseButtonEntryState>();
	}

	FVector MouseMoveState::GetMouseProjection(const ABasePlayerController& controller) {
		FHitResult hitResult;
		controller.GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::PlayerPlane), false, hitResult);

		return hitResult.Location;
	}

	TOptional<FVector> MouseMoveState::GetSafeDestination(Terrain* terrain, FVector source, FVector destination) {		
		const auto orientation = (destination - source).GetSafeNormal2D();

		const auto forward = source + orientation * 50.f * 1.4142135623730950488016887242097f/*FMath::Sqrt(2.f)*/;
		if (!CVarAvoidKillzones.GetValueOnGameThread() || !GetTerrainType(terrain, forward).isKillzone()) {
			return { destination };
		} else {
			const FVector candidateHorizontal {
				source.X + (orientation.X > 0.f ? 100.f : -100.f),
				FMath::FloorToFloat(source.Y / 100.f) * 100.f + (orientation.Y > 0.f ? 70.f : 30.f),
				source.Z
			};

			const FVector candidateVertial {
				FMath::FloorToFloat(source.X / 100.f) * 100.f + (orientation.X > 0.f ? 70.f : 30.f),
				source.Y + (orientation.Y > 0.f ? 100.f : -100.f),
				source.Z
			};

			const bool isHorizontal = FMath::Abs(orientation.X) > FMath::Abs(orientation.Y);
			const FVector& guessFirst = isHorizontal ? candidateHorizontal : candidateVertial;

			if (!GetTerrainType(terrain, guessFirst).isKillzone()) {
				return guessFirst;
			} else {
				const FVector& guessSecond = isHorizontal ? candidateVertial : candidateHorizontal;

				if (!GetTerrainType(terrain, guessSecond).isKillzone()) {
					return { guessSecond };
				} else {
					return {};
				}
			}
		}
	}

	void MouseMoveState::MoveInDirection(ABasePlayerController& controller,const FVector& direction) {
		controller.GetPawn()->AddMovementInput(direction);
		controller.SetControlRotation(direction.ToOrientationRotator());
	}

	MouseInputStateTypeId MouseMoveState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (Gamepad)
		{
			return TickGamepad(deltaTime, controller, targetController, mouseState);
		}
		else
		{
			return TickMouse(deltaTime, controller, targetController, mouseState);
		}
	}

	input::MouseInputStateTypeId MouseMoveState::TickMouse(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (mouseState.MoveButton.Pressed) {
			FVector direction =  GetMouseProjection(controller) - controller.GetPawn()->GetActorLocation();
			direction.Normalize();
			MoveInDirection(controller, direction);
			
			bool shouldPathfindOnEnd = mouseState.MoveButton.TimeInState <= MouseInputStateMachine::GetClickGraceTimeSeconds();

			if (shouldPathfindOnEnd != PathfindOnEnd) {
				PathfindOnEnd = shouldPathfindOnEnd;
			}

			if (mouseState.RootButton.Pressed) {
				if (controller.IsCustomMovementTypeActive(ECustomMovementType::Gliding)) {
					return GetId<MouseButtonElytraDiveState>();
				}
				
				return GetId<MouseStationaryMeleeState>();
			}
		}

		return UndefinedState;
	}

	input::MouseInputStateTypeId MouseMoveState::TickGamepad(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		// D11.DB
		if (mouseState.RangedButton.Pressed && MouseRangedState::CanEnterState(controller))
		{
			return GetId<MouseRangedState>();
		}
		if (mouseState.MeleeButton.Pressed)
		{
			return GetId<MouseButtonEntryState>();
		}
		if (mouseState.RootButton.Pressed)
		{
			return GetId<MouseButtonEntryState>();
		}
		if (mouseState.MoveAxis.Active) 
		{
			MoveInDirection(controller, controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.f), 100.f, false));
			return UndefinedState;
		}

		// D11.DB - This is a necessary call as it helps improve movement controls when on a slippery surface.
		controller.GetPathFollowingComponent()->AbortMove(controller, FPathFollowingResultFlags::AlreadyAtGoal, false);

		return GetId<MouseButtonEntryState>();
	}
}

