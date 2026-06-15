#pragma once

#include <tuple>
#include "MouseInputState.h"
#include "MouseButtonEntryState.h"
#include "MouseContinuousMeleeState.h"
#include "MouseMeleeState.h"
#include "MouseRangedState.h"
#include "MouseMoveState.h"
#include "MouseButtonClickyState.h"
#include "MouseStationaryMeleeState.h"
#include "MouseTargetingState.h"
#include "MouseCursorStates.h"
#include "MouseDynamicRootState.h"
#include "MouseButtonElytraDiveState.h"
#include <chrono>


class ABasePlayerController;
class TargetController;
	
namespace input {

	class MouseInputStateMachine {
		using StorageType = std::tuple<MouseButtonEntryState, MouseContinuousMeleeState, MouseMeleeState, MouseRangedState, MouseMoveState, MouseButtonClickyState, MouseStationaryMeleeState, MouseTargetingState, MouseDynamicRootState, MouseButtonElytraDiveState>;
		StorageType Storage;
		SMouseState MouseState;
		IMouseInputState* CurrentState;

		IMouseInputState* GetState(MouseInputStateTypeId wantedType) {
			IMouseInputState* state = GetStateImpl<std::tuple_size<StorageType>() - 1>(wantedType);
			check(state && "State not present in state machine!");
			return state;
		}

		template <int I> IMouseInputState* GetStateImpl(MouseInputStateTypeId wantedType) {
			auto& element = std::get<(size_t)I>(Storage);

			if (GetIdWithArgument(element) == wantedType) {
				return &element;
			}

			return GetStateImpl<I - 1>(wantedType);
		}
		//D11.PS - moved this below
		//template <> IMouseInputState* GetStateImpl<-1>(MouseInputStateTypeId wantedType) {
		//	return nullptr;
		//}

		bool stateChangeAllowed = true;

	public:
		MouseInputStateMachine();

		void OnMoveButton(bool pressed, ABasePlayerController& controller, TargetController& targetController);
		void OnMoveAxis(float x, float y, ABasePlayerController& controller, TargetController& targetController); // D11.DB
		void OnRangedButton(bool pressed, ABasePlayerController& controller, TargetController& targetController);
		void OnGamepadRangedButton(bool pressed, ABasePlayerController& controller, TargetController& targetController);
		void OnRootButton(bool pressed, ABasePlayerController& controller, TargetController& targetController);
		void OnGamepadRootButton(bool pressed, ABasePlayerController& controller, TargetController& targetController);
		void OnMeleeButton(bool pressed, bool gamepad, ABasePlayerController& controller, TargetController& targetController); // D11.DB
		void Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController);

		void ChangeState(MouseInputStateTypeId newStateId, ABasePlayerController& controller, TargetController& targetController);

		void Reset(ABasePlayerController& controller, TargetController& targetController);

		void ForceIncrementMouseVersion();

		EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController);

		static const std::chrono::milliseconds ClickGraceTime;
		static float GetClickGraceTimeSeconds();

		void AllowStateChange(bool allow);
	};

	//D11.PS - moved this here
	template <> inline IMouseInputState* MouseInputStateMachine::GetStateImpl<-1>(MouseInputStateTypeId wantedType) {
		return nullptr;
	}

}