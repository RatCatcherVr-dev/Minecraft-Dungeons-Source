#pragma once

#include "game/ai/bt/BtTypes.h"

enum class EMovementState : uint8;

namespace bt { namespace speed {

struct Speed {
	Speed(Absolute absolute) : absolute(absolute) {}
	Speed(Relative relative) : relative(relative) {}
	Speed(EMovementState movement, float relative = 1.0f) : relative(relative), movement(movement) {}

	TOptional<float> absolute;
	TOptional<float> relative;
	TOptional<EMovementState> movement;
};

Action SetAbsolute(float speed);
Action SetAbsolute(const Provider<float>& speedProvider);
Action SetRelative(float relativeFraction);
Action SetRelative(EMovementState, float relative = 1.0f);
Action Set(const Speed&);

}}
