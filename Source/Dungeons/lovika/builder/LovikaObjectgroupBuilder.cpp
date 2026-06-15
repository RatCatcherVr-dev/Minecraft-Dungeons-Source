#include "Dungeons.h"

#include "LovikaObjectgroupBuilder.h"

#include "RuntimeMeshComponent.h"

#include "ObjectGroupBlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/Block.h"
#include "lovika/io/ObjectGroupFile.h"
#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/block/BlockTessellator.h"
#include "game/level/instancing/InstancedLeavesActor.h"
#include "game/util/ActorQuery.h"
#include "BlockInfo.h"
#include "UObjectGlobals.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "util/StringUtil.h"

void ULovikaObjectgroupBuilder::build(bool useAmbientOcclusion, FString objectGroupName, bool rebuildMeshes, const TOptional<TArray<FString>>& allowedTileIds) {
	FString path = FPaths::ProjectDir() + "Content/data/lovika/objectgroups/" + objectGroupName;

	if (!FPaths::FileExists(path)) {
		UE_LOG(LogTemp, Warning, TEXT("File does not exist: %s"), *path);
		return;
	}

	FString contents;
	FFileHelper::LoadFileToString(contents, *path);

	auto levelDataUtf = std::string(TCHAR_TO_UTF8(*contents));
	std::istringstream in(levelDataUtf);

	build(useAmbientOcclusion, io::loadObjectGroup(in, stringutil::toStdString(objectGroupName)), rebuildMeshes, allowedTileIds);
}

void ULovikaObjectgroupBuilder::build(
	bool useAmbientOcclusion,
	Unique<io::ObjectGroup> objects,
	bool rebuildMeshes,
	const TOptional<TArray<FString>>& allowedTileIds
) {
	using namespace builder;
	
	auto owner = GetOwner();

	objectGroup = std::move(objects);

	// Create dummy storage
	Tessellator tesselator;
	region = new ObjectGroupBlockSource(*objectGroup);

	if (!rebuildMeshes) {
		return;
	}

	TArray<AActor*> childActors;
	owner->GetAttachedActors(childActors);
	for (AActor* actor : childActors) {
		actor->Destroy();
	}
	for (UActorComponent* meshComponent : owner->GetComponentsByClass(URuntimeMeshComponent::StaticClass())) {
		meshComponent->DestroyComponent();
	}

	/*
	 * We probably need to do something like in the other place where we loop over all the resource packs we have
	 * activated and then loop over those tiles per tessellator or something...
	 */
	BlockTessellator renderer(tesselator, BlockGraphicsHelper::getInstance(GetWorld()).getLatest(), region, RENDERCHUNK_SIDE);
	renderer.setUnrealActor(owner);
	renderer.setSmoothLightingEnabled(useAmbientOcclusion);

	auto* instancedLeavesActor = actorquery::getFirstActor<AInstancedLeavesActor>(GetWorld());

	int nrMeshes = 0;
	int sectionIndex = 0;
	for (auto& object : objectGroup->objects) {
		if (isTileAllowed(object.id, allowedTileIds)) {
			const auto& bounds = object.bounds;
			const BlockPos min = bounds.minInclusive;
			const BlockPos max = bounds.maxExclusive;

			if (instancedLeavesActor) {
				instancedLeavesActor->Place(min, *object.blocks);
			}

			BlockPos diff = max - min;
			int chunkSizeX = FMath::CeilToInt((float)diff.x / RENDERCHUNK_SIDE);
			int chunkSizeY = FMath::CeilToInt((float)diff.y / RENDERCHUNK_SIDE);
			int chunkSizeZ = FMath::CeilToInt((float)diff.z / RENDERCHUNK_SIDE);

			for (int chunkX = 0; chunkX < chunkSizeX; chunkX++) {
				for (int chunkY = 0; chunkY < chunkSizeY; chunkY++) {
					for (int chunkZ = 0; chunkZ < chunkSizeZ; chunkZ++) {

						TMap<BlockRenderLayer, TArray<BlockInfo>> infos;

						int maxX = FMath::Min(diff.x - chunkX * RENDERCHUNK_SIDE, RENDERCHUNK_SIDE);
						int maxY = FMath::Min(diff.y - chunkY * RENDERCHUNK_SIDE, RENDERCHUNK_SIDE);
						int maxZ = FMath::Min(diff.z - chunkZ * RENDERCHUNK_SIDE, RENDERCHUNK_SIDE);

						BlockPos currentMin(min.x + chunkX * RENDERCHUNK_SIDE, min.y + chunkY * RENDERCHUNK_SIDE, min.z + chunkZ * RENDERCHUNK_SIDE);

						for (int x = 0; x < maxX; x++) {
							for (int z = 0; z < maxZ; z++) {
								for (int y = 0; y < maxY; y++) {
									BlockPos pos(x + chunkX * RENDERCHUNK_SIDE, y + chunkY * RENDERCHUNK_SIDE, z + chunkZ * RENDERCHUNK_SIDE);

									FullBlock fullBlock = object.blocks->getBlock(pos);
									const Block* block = Block::mBlocks[(int)fullBlock.id];
									if (block != nullptr && !block->isType(Block::mAir)) {
										BlockPos realPos(x + currentMin.x, y + currentMin.y, z + currentMin.z);

										if (infos.Contains(block->getRenderLayer())) {
											infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, realPos));
										} else {
											TArray<BlockInfo> infoList;
											infoList.Add(BlockInfo(fullBlock, realPos));
											infos.Add(block->getRenderLayer(), infoList);
										}
									}
								}
							}
						}

						URuntimeMeshComponent* mesh = nullptr;
						for (auto& blockInfo : infos) {
							auto l = blockInfo.Key;
							TArray<BlockInfo>& blocks = blockInfo.Value;

							renderer.setRenderLayer(l);
							renderer.setForceOpaque(l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
							renderer.resetCache(currentMin, *region);
							renderer.clear();

							for (auto& bb : blocks) {
								const Block* block = Block::mBlocks[(int)bb.fullBlock.id];
								renderer.tessellateInWorld(*block, bb.pos, bb.fullBlock.data);
							}

							const auto& blockGraphicsPack = renderer.getBlockGraphicsPack();

							if (tesselator.vertices.Num() > 0) {
								bool generateCollision = true;
								bool castsShadow = true;
								UMaterialInterface* currentMaterial = blockGraphicsPack.atlasMaterialInstanceOpaque.Get();

								switch (l) {
								case BlockRenderLayer::RENDERLAYER_WATER:
									currentMaterial = materialWater;
									generateCollision = false;
									break;
								case BlockRenderLayer::RENDERLAYER_LAVA:
									currentMaterial = materialEmissive;
									generateCollision = false;
									break;
								case BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE:
									currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlpha.Get();
									generateCollision = true;
									castsShadow = false;
									break;
								case BlockRenderLayer::RENDERLAYER_ALPHATEST:
								case BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST:
								case BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED:
								case BlockRenderLayer::RENDERLAYER_BLEND:
								case BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST:
									currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlpha.Get();
									generateCollision = false;
									castsShadow = false;
									break;
								case BlockRenderLayer::RENDERLAYER_OPAQUE_ADDITIONAL_ZEROALPHA_GEOMETRY:
									currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlpha.Get();
									break;
								case BlockRenderLayer::RENDERLAYER_ALPHATEST_FOILAGE:
									currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlphaFoilage.Get();
									generateCollision = false;
									castsShadow = false;
									break;
								case BlockRenderLayer::RENDERLAYER_TORCH:
									generateCollision = false;
									break;
								case BlockRenderLayer::RENDERLAYER_FIRE:
									generateCollision = false;
									break;
								case BlockRenderLayer::RENDERLAYER_LEAVES:
									currentMaterial = blockGraphicsPack.atlasMaterialInstanceLeaves.Get();
									generateCollision = false;
									break;
								case BlockRenderLayer::RENDERLAYER_VOID:
									currentMaterial = materialVoid;
									generateCollision = false;
									break;
								default:
									/* Empty */
									break;
								}

								if (mesh == nullptr) {
									mesh = NewObject<URuntimeMeshComponent>(owner, FName(("Mesh_" + currentMin.toString()).c_str()));
									mesh->RegisterComponent();
									mesh->AttachToComponent(owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
									mesh->SetMobility(EComponentMobility::Static);
									const int scale = 100;
									mesh->SetRelativeScale3D(FVector(scale, scale, scale));
									mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
									mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
									mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel10, ECollisionResponse::ECR_Block);
									sectionIndex = 0;
								}


								mesh->CreateMeshSection(sectionIndex, 0, tesselator.vertices, tesselator.indices, tesselator.normals, tesselator.uv1, tesselator.uv2, tesselator.colors, TArray<FRuntimeMeshTangent>(), generateCollision, EUpdateFrequency::Infrequent);
								mesh->SetMaterial(sectionIndex, currentMaterial);
								mesh->SetMeshSectionCastsShadow(sectionIndex, castsShadow);
								sectionIndex++;
								nrMeshes++;
							}
						}
					}
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Total nr meshes: %d"), nrMeshes);
	}
}

bool ULovikaObjectgroupBuilder::isTileAllowed(const std::string& tileId,
                                             const TOptional<TArray<FString>>& allowedTileIds)
{	
	if (allowedTileIds)
	{
		const auto tileIdValues = allowedTileIds.GetValue();
		if (tileIdValues.Num() > 0) {
			for (auto& allowedTileId : tileIdValues)
			{
				if (tileId.compare(std::string(TCHAR_TO_UTF8(*allowedTileId))) == 0)
				{
					return true;
				}
			}
			return false;
		}		
	}
	return true;
}

io::ObjectGroup* ULovikaObjectgroupBuilder::getObjectGroupPtr() {
	return objectGroup.get();
}