#include "Dungeons.h"
#include "BlockQuery.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"

namespace blockquery {
	bool isLogicallySolid(const Block& block) {
		const auto& materialType = block.getMaterial().getType();
		return !(materialType == MaterialType::Plant
			|| materialType == MaterialType::ReplaceablePlant
			|| materialType == MaterialType::Leaves
			|| materialType == MaterialType::Air);
	}
}
