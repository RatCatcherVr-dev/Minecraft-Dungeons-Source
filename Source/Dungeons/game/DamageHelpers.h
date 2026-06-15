#pragma once

class AActor;
class BlockPos;

namespace game {
	class Tiles;
}

enum class ECharacterBoundsCheckPenalty {
	Nothing,
	Death,
	KillZoneLike
};

namespace damagehelpers {

bool tryKillByKillZone(AActor&, bool isFreezeZone);
ECharacterBoundsCheckPenalty evaluateBoundsCheckPenalty(const game::Tiles&, const BlockPos&);

}
