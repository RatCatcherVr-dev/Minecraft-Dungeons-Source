#pragma once
#include "lovika/world/level/terrain/Terrain.h"
#include "game/input/MouseInputState.h"

namespace input {

	class MouseMoveState : public IMouseInputState {
		bool PathfindOnEnd = false;
		bool Gamepad = false;
	public:
		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
		void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override; // D11.DB
		MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId TickMouse(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);
		MouseInputStateTypeId TickGamepad(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);
		
		EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;
		MouseInputStateTypeId GetInterruptState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;		
		
	private:
		static terrain::Type GetTerrainType(Terrain* terrain, const FVector& location);
		static bool IsDirectSafe(Terrain* terrain, FVector from, FVector to);
		static bool IsComplexPath(const TArray<FNavPathPoint>& points);
		static void PathTowardsCursor(ABasePlayerController& controller, bool simplePath);
		static TOptional<FVector> GetSafeDestination(Terrain* terrain, FVector source, FVector destination);
		static FVector GetMouseProjection(const ABasePlayerController& controller);
		static void MoveInDirection(ABasePlayerController& controller, const FVector& direction);
	};
}