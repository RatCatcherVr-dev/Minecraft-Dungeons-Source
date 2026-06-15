#pragma once

UENUM(BlueprintType)
enum class EMouseCursorStates : uint8 {
	UNSET,
	Move,
	MoveHeld,
	Attack,
	AttackHeld,
	Interact,
	InteractHeld,
	Drag,
	DragHeld,
	Invisible, // D11.DB
};