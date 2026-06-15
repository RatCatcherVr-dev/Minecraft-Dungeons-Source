#pragma once

#include "MouseCursorStates.h"

class ABasePlayerController;
class TargetController;

namespace input {
	using MouseInputStateTypeId = size_t;
	const MouseInputStateTypeId UndefinedState = 0;

	template <typename T> MouseInputStateTypeId GetId() {
		static int id = 0;

		return reinterpret_cast<MouseInputStateTypeId>(&id);
	}

	template <typename T> MouseInputStateTypeId GetIdWithArgument(const T&) {
		return GetId<T>();
	}


struct SButtonState {
	bool Pressed = false;	
	float TimeInState = 0.f;
	unsigned int PressVersion = 0;	
	bool Gamepad = false;

	void Update(bool pressed, bool gamepad = false) {		
		if (Pressed != pressed) {
			Pressed = pressed;
			TimeInState = 0.f;
			if (pressed) {
				PressVersion ++;
			}
		}
		Gamepad = gamepad;
	}

	bool IsBeingHeld() const {
		return TimeInState > 0.0f;
	}

	void Reset() {
		Pressed = false;
		TimeInState = 0;
		// Version is intentionally left out
	}
};

// D11.DB
struct SAxisState
{
	float X;
	float Y;
	bool Active;

	void Update(float x, float y)
	{
		X += x;
		Y += y;

		X = FMath::Clamp(X, -1.0f, 1.0f);
		Y = FMath::Clamp(Y, -1.0f, 1.0f);
		
		Active = !FMath::IsNearlyZero(X) || !FMath::IsNearlyZero(Y);
	}

	void Reset()
	{
		X = 0.0f;
		Y = 0.0f;
		Active = false;
	}
};
// D11.DB - END

struct SMouseState {
	SButtonState MoveButton;
	SButtonState RangedButton;
	SButtonState RootButton;
	
	// D11.DB
	SAxisState MoveAxis;
	SButtonState MeleeButton;
};

class IMouseInputState {
public:
	virtual MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { return UndefinedState; }
	virtual MouseInputStateTypeId OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { return UndefinedState; }
	virtual MouseInputStateTypeId OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { return UndefinedState; } // D11.DB
	virtual MouseInputStateTypeId OnRootButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { return UndefinedState; } // D11.KS
	virtual MouseInputStateTypeId OnMoveAxis(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { return UndefinedState; } // D11.DB

	virtual MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { return UndefinedState; }

	virtual MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) { return UndefinedState; }
	virtual void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) { }

	virtual EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const { return EMouseCursorStates::UNSET; }
	virtual MouseInputStateTypeId GetInterruptState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const { return UndefinedState; }
};
}