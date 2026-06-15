#pragma once

#include "world/level/BlockPos.h"
#include <GameFramework/Actor.h>
#include <map>
#include "StrongObjectPtr.h"

#include "InstancedLeavesActor.generated.h"

class UStaticMesh;
class UInstancedStaticMeshComponent;

namespace game { class Tile; using TileRef = const Tile&; }

class Block;
class BlockRegion;
class BlockGraphicsPack;

struct Key {
	const BlockGraphicsPack* blockGraphicsPack;
	uint32 textureKey;

	bool operator<(const Key& rhs) const {
		return std::tie(blockGraphicsPack, textureKey) < std::tie(rhs.blockGraphicsPack, rhs.textureKey);
	}
};

UCLASS()
class DUNGEONS_API AInstancedLeavesActor : public AActor {
	GENERATED_BODY()
public:
	void Place(game::TileRef);
	void Place(BlockPos base, const BlockRegion&);
private:
	bool _isEnabledAndInited();
	UInstancedStaticMeshComponent& _getOrCreateInstancedMesh(FullBlock, BlockPos);
	UInstancedStaticMeshComponent& _getOrCreateInstancedMesh(FullBlock, const BlockGraphicsPack&);

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh = nullptr;

	std::map<Key, UInstancedStaticMeshComponent*> Instances;
};
