#pragma once
#include <Platform.h>

namespace missions {  namespace hajper {

enum class DungeonEligibility : uint8 {
	CAN_PLAY_DUNGEON,
	COULD_PLAY_IF_YOU_HAD_DLC,
	CANNOT_PLAY_DUNGEON,
};

}}
