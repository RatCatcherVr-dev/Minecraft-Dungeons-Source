#include "Dungeons.h"
#include "InstancedLeavesActor.h"
#include "game/Conversion.h"
#include "game/level/GameTile.h"
#include "game/util/ActorQuery.h"
#include "game/util/ComponentUtils.h"
#include "lovika/BlockRegion.h"
#include "lovika/LovikaLevelActor.h"
#include "lovika/LovikaObjectgroupLevelActor.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include <Components/InstancedStaticMeshComponent.h>
#include <Components/HierarchicalInstancedStaticMeshComponent.h>

TAutoConsoleVariable<int32> CVarInstancedLeaves(
	TEXT("Dungeons.Instancing.Leaves"),
	1,
	TEXT("Enable/Disable instanced leaves")
	TEXT("<= 0: off.\n")
	TEXT(">= 1: on.\n"),
	ECVF_Cheat);


template <typename BlockPosFullBlockFn>
void forAllLeaves(const BlockRegion& region, const BlockPosFullBlockFn& leavesCallback) {
	for (auto pos : BlockPosIteration::range(region.size())) {
		const auto fullBlock = region.getBlock(pos);
		const auto* block = Block::mBlocks[fullBlock.id];
		if (block && block->hasProperty(BlockProperty::Leaf)) {
			leavesCallback(pos, fullBlock);
		}
	}
}

void AInstancedLeavesActor::Place(game::TileRef tile) {
	const auto blockPosTransform = tile.tilePlacement().blockPosTransform();

	forAllLeaves(tile.tile().blocks(), [this, blockPosTransform, &tile](BlockPos pos, FullBlock fullBlock) {
		_getOrCreateInstancedMesh(fullBlock, tile.dungeon().blockGraphicsPack(GetWorld())).AddInstance(FTransform(conversion::blockCenterXYZToUe(blockPosTransform(pos))));
	});
}

void AInstancedLeavesActor::Place(BlockPos base, const BlockRegion& blocks) {
	forAllLeaves(blocks, [this, base](BlockPos pos, FullBlock fullBlock) {
		_getOrCreateInstancedMesh(fullBlock, pos).AddInstance(FTransform(conversion::blockCenterXYZToUe(base + pos)));
	});
}

UInstancedStaticMeshComponent& AInstancedLeavesActor::_getOrCreateInstancedMesh(FullBlock block, BlockPos pos) {
	return _getOrCreateInstancedMesh(block, BlockGraphicsHelper::getBlockGraphicsPack(GetWorld(), pos));
}

UInstancedStaticMeshComponent& AInstancedLeavesActor::_getOrCreateInstancedMesh(FullBlock block, const BlockGraphicsPack& blockGraphicsPack) {
	const BlockGraphics& blockGraphics = blockGraphicsPack.get(block.id);

	const auto& textureCoordinates = blockGraphics.getCarriedTexture(Facing::DOWN, Block::mBlocks[block.id]->getVariant(block.data));
	const auto pu0 = textureCoordinates.asPixelU0();
	const auto pv0 = textureCoordinates.asPixelV0();
	const uint32 textureKey = (pu0 << 16) | pv0;

	Key key = { &blockGraphicsPack, textureKey };
	auto instance = Instances.find(key);
	if (instance != Instances.end()) {
		return *instance->second;
	}

	UInstancedStaticMeshComponent* instancedMesh = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, UHierarchicalInstancedStaticMeshComponent::StaticClass());
	instancedMesh->SetStaticMesh(Mesh);
	instancedMesh->bUseDefaultCollision = true;
	instancedMesh->RegisterComponent();

	auto* materialInstance = instancedMesh->CreateDynamicMaterialInstance(0, instancedMesh->GetMaterial(0));
	materialInstance->SetTextureParameterValue(FName("T2DParam"), blockGraphicsPack.terrainTextureAtlas->atlasTexture);
	materialInstance->SetScalarParameterValue(FName("OffsetU"), textureCoordinates._u0);
	materialInstance->SetScalarParameterValue(FName("OffsetV"), textureCoordinates._v0);
	materialInstance->SetScalarParameterValue(FName("ScaleU"), textureCoordinates._u1 - textureCoordinates._u0);
	materialInstance->SetScalarParameterValue(FName("ScaleV"), textureCoordinates._v1 - textureCoordinates._v0);
	instancedMesh->SetMaterial(0, materialInstance);
	Instances.insert({ key, instancedMesh });
	return *instancedMesh;
}
