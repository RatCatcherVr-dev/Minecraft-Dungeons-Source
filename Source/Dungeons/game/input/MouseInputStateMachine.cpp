#include "Dungeons.h"
#include "MouseInputStateMachine.h"
#include "game/actor/character/player/BasePlayerController.h"


namespace input {

MouseInputStateMachine::MouseInputStateMachine() : Storage(), CurrentState(GetState(GetId<MouseButtonEntryState>())) {
}

void MouseInputStateMachine::OnMoveButton(bool pressed, ABasePlayerController& controller, TargetController& targetController) {
	MouseState.MoveButton.Update(pressed);
	ChangeState(CurrentState->OnMoveButtonStateChanged(controller, targetController, MouseState), controller, targetController);
}

// D11.DB
void MouseInputStateMachine::OnMoveAxis(float x, float y, ABasePlayerController& controller, TargetController& targetController) {
	MouseState.MoveAxis.Update(x, y);
	ChangeState(CurrentState->OnMoveAxis(controller, targetController, MouseState), controller, targetController);
}

void MouseInputStateMachine::OnRangedButton(bool pressed, ABasePlayerController& controller, TargetController& targetController) {
	MouseState.RangedButton.Update(pressed);
	ChangeState(CurrentState->OnRangedButtonStateChanged(controller, targetController, MouseState), controller, targetController);
}

void MouseInputStateMachine::OnGamepadRangedButton(bool pressed, ABasePlayerController& controller, TargetController& targetController) {
	MouseState.RangedButton.Update(pressed, true);
	ChangeState(CurrentState->OnRangedButtonStateChanged(controller, targetController, MouseState), controller, targetController);
}

void MouseInputStateMachine::OnRootButton(bool pressed, ABasePlayerController& controller, TargetController& targetController) {
	MouseState.RootButton.Update(pressed);
	ChangeState(CurrentState->OnRootButtonStateChanged(controller, targetController, MouseState), controller, targetController);
}

void MouseInputStateMachine::OnGamepadRootButton(bool pressed, ABasePlayerController& controller, TargetController& targetController)
{
	MouseState.RootButton.Update(pressed, true);
	ChangeState(CurrentState->OnRootButtonStateChanged(controller, targetController, MouseState), controller, targetController);
}

void MouseInputStateMachine::OnMeleeButton(bool pressed, bool gamepad, ABasePlayerController& controller, TargetController& targetController)
{
	MouseState.MeleeButton.Update(pressed, gamepad);
	ChangeState(CurrentState->OnMeleeButtonStateChanged(controller, targetController, MouseState), controller, targetController);
}

void MouseInputStateMachine::ChangeState(MouseInputStateTypeId newStateId, ABasePlayerController& controller, TargetController& targetController) {
#if WITH_EDITOR
	int counter = 0;
#endif

	if (stateChangeAllowed) {
		while (newStateId != UndefinedState) {
			auto nextState = GetState(newStateId);
			auto oldState = CurrentState;
			CurrentState->OnExitState(*nextState, controller, targetController, MouseState);
			CurrentState = nextState;
			newStateId = nextState->OnEnterState(controller, targetController, *oldState, MouseState);
#if WITH_EDITOR
			check(counter++ < std::tuple_size<StorageType>() && "Infinite state transition loop in input controller.");
#endif
		}
	}
}

void MouseInputStateMachine::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController) {
	ChangeState(CurrentState->Tick(deltaTime, controller, targetController, MouseState), controller, targetController);
	MouseState.RangedButton.TimeInState += deltaTime;
	MouseState.MoveButton.TimeInState += deltaTime;
	MouseState.MeleeButton.TimeInState += deltaTime;

	// D11.DB - Axis needs reseting every tick.
	MouseState.MoveAxis.Reset();
}

void MouseInputStateMachine::Reset(ABasePlayerController& controller, TargetController& targetController) {
	MouseState.MeleeButton.Reset();
	MouseState.MoveAxis.Reset();
	MouseState.MoveButton.Reset();
	MouseState.RangedButton.Reset();
	MouseState.RootButton.Reset();
	ChangeState(GetId<MouseButtonEntryState>(), controller, targetController);
}

EMouseCursorStates MouseInputStateMachine::GetCursorState(ABasePlayerController& controller, TargetController& targetController) {
	auto CursorState = CurrentState->GetCursorState(controller, targetController, MouseState);	
	if (CursorState != EMouseCursorStates::UNSET) {
		return CursorState;
	}
	auto InterruptState = CurrentState->GetInterruptState(controller, targetController, MouseState);
	if(InterruptState != UndefinedState){
		CursorState = GetState(InterruptState)->GetCursorState(controller, targetController, MouseState);
	}
	return CursorState;
};

const std::chrono::milliseconds MouseInputStateMachine::ClickGraceTime = std::chrono::milliseconds(200);

float MouseInputStateMachine::GetClickGraceTimeSeconds() {
	return std::chrono::duration_cast<std::chrono::duration<float>>(ClickGraceTime).count();
}

void MouseInputStateMachine::ForceIncrementMouseVersion() {
	//Hack to get around UI consuming clicks in certain cases.
	MouseState.MoveButton.PressVersion++;
	MouseState.RangedButton.PressVersion++;
	MouseState.MeleeButton.PressVersion++;
}

// D11.SSN - blocks/allows mouse input state from being changed
void MouseInputStateMachine::AllowStateChange(bool allow) {
	stateChangeAllowed = allow;
}

}
