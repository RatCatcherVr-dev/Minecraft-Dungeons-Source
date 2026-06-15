#include "Dungeons.h"

#include "LovikaGeneratorBuilder.h"
#include "BlockInfo.h"
#include "lovika/io/WalkableHeight.h"
#include "lovika/RegionPredicates.h"
#include "lovika/world/level/ChunkBlockSource.h"
#include "lovika/world/level/levelgen/LevelGen.h"
#include "lovika/world/level/postprocess/OutsideFiller.h"
#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/block/BlockTessellator.h"
#include "client/renderer/renderer/BoundaryTesselator.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "game/util/ActorQuery.h"
#include "game/util/EnvironmentUtils.h"
#include "game/util/ComponentUtils.h"
#include "game/navigation/NavAreaLava.h"
#include "util/CollectionUtils.h"
#include <numeric>
#include <algorithm>
#include "NavModifierComponent.h"


#define DO_SIMPLE_SHADOW_MESHES (!WITH_EDITOR)
#define SHOW_LOAD_TIMES 0
#define EDITOR_SHADOW_TESTING 0

#if SHOW_LOAD_TIMES
#define SHOW_LOAD_TIME_CALL(x) x
#else
#define SHOW_LOAD_TIME_CALL(x)
#endif

TAutoConsoleVariable<int32> CVarHardBackFaceCulling(
	TEXT("Dungeons.Perf.BackFaceCulling"),
	1,
	TEXT("Enables \"hard\" back face culling in that the geometry is actually removed\n")
	TEXT("<=  0: disabled\n")
	TEXT("    1: enabled for game-determined areas\n")
	TEXT(">=  2: always enabled (useful for debug only)\n"),
	ECVF_Default
);

TAutoConsoleVariable<int32> CVarSimpleCollisionGeometry(
	TEXT("Dungeons.level.UseSimpleCollisionGeometry"),
	1,
	TEXT("Enable/Disable Simple collision Geometry")
	TEXT("<= 0: off.\n")
	TEXT(">= 1: on.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarBoundryGridSize(
	TEXT("Dungeons.level.BoundryGridSize"),
	4,
	TEXT("Determines the grid size to split the bounrdy collision volumes into"),
	ECVF_Cheat);

int32 ShowInvisibleWalls = 0;
FAutoConsoleVariableRef CVarShowInvisibleWalls(
	TEXT("Dungeons.level.ShowInvisibleWalls"),
	ShowInvisibleWalls,
	TEXT("Enable/Disable visibility of invisible walls - BLUE")
	TEXT("<= 0: off.\n")
	TEXT(">= 1: on.\n"),
	ECVF_Cheat);

float InvisibleWallsAlpha = 0.1f;
FAutoConsoleVariableRef CVarInvisibleWallsAlpha(
	TEXT("Dungeons.level.InvisibleWallsAlpha"),
	InvisibleWallsAlpha,
	TEXT("Invisible walls alpha value. Range: 0.0 to 1.0.\n"),
	ECVF_Cheat);

int32 ShowKillZones = 0;
FAutoConsoleVariableRef CVarShowKillZones(
	TEXT("Dungeons.level.ShowKillZones"),
	ShowKillZones,
	TEXT("Enable/Disable visibility of kill zones & outside tile blockers - RED. <= 0: off, >= 1: on.\n"),
	ECVF_Cheat);

float KillZoneAlpha = 0.1f;
FAutoConsoleVariableRef CVarKillZoneAlpha(
	TEXT("Dungeons.level.KillZoneAlpha"),
	KillZoneAlpha,
	TEXT("Kill zone alpha value. Range: 0.0 to 1.0.\n"),
	ECVF_Cheat);


namespace builder {

#if PLATFORM_SWITCH
	constexpr bool UseFullPrecisionUVs{ false };
#else
	constexpr bool UseFullPrecisionUVs{ true };
#endif 

	bool IsValidMaterial(UMaterialInterface* material)
	{
		return material != nullptr && !material->GetFName().IsNone() && material->GetFName().IsValid();
	};

std::vector<BlockCuboid> outerRimOf(BlockPos s) {
	const auto narrowestDimension = std::min(s.x, s.z);

	if (narrowestDimension <= 2) {
		return { BlockCuboid::fromPositionAndSize({0, 0, 0}, {s.x, s.y, s.z}) };
	}
	return {
		BlockCuboid::fromPositionAndSize({ 0,      0, 0 }, { 1, s.y, s.z }),
		BlockCuboid::fromPositionAndSize({ s.x - 1, 0, 0 }, { 1, s.y, s.z }),
		BlockCuboid::fromPositionAndSize({ 1, 0,      0 }, { s.x - 2, s.y, 1 }),
		BlockCuboid::fromPositionAndSize({ 1, 0, s.z - 1 }, { s.x - 2, s.y, 1 })
	};
}

std::vector<BlockCuboid> outerRimOf(const BlockCuboid& volume) {
	return algo::map_vector(outerRimOf(volume.size()), RETLAMBDA(it + volume.minInclusive));
}

std::vector<BlockPos> rimBlocksXzOutsideOf(const BlockCuboid& volume) {
	std::vector<BlockPos> outside;

	for (const auto& rimVolume : outerRimOf(flatten(growed(volume, BlockPos::ONE)))) {
		algo::map_to(rimVolume, RETLAMBDA(it), outside);
	}
	return outside;
}

	meshinfo::MeshSectionInfo CreateMeshSection(Tessellator& tessellator, bool generateCollision, bool castsShadow, UMaterialInterface* currentMaterial, int sectionIndex)
	{
		meshinfo::MeshSectionInfo meshSectionInfo;
		meshSectionInfo.sectionIndex = sectionIndex;
		meshSectionInfo.bGenerateComplexCollision = generateCollision;
		meshSectionInfo.bCastShadow = castsShadow;
		meshSectionInfo.material = currentMaterial;

		meshSectionInfo.meshData = std::make_unique<meshinfo::MeshSectionData>();
		meshinfo::MeshSectionData& meshSectionData = *meshSectionInfo.meshData;

		meshSectionData.vertices = std::move(tessellator.vertices);
		meshSectionData.indices = std::move(tessellator.indices);
		meshSectionData.normals = std::move(tessellator.normals);
		meshSectionData.uv1 = std::move(tessellator.uv1);
		meshSectionData.colors = std::move(tessellator.colors);		

		return meshSectionInfo;
	}


	meshinfo::MeshInfo CreateMeshInfo(meshinfo::EMeshType meshType, int meshNum, const BlockPos& currentMin, const meshinfo::MeshInfo::SharedData& data)
	{
		meshinfo::MeshInfo meshInfo;
		meshInfo.meshType = meshType;
		meshInfo.meshNum = meshNum;
		meshInfo.position = currentMin;
		meshInfo.sharedData = data;
		return meshInfo;
	}

	meshinfo::MeshInfo CreateMeshInfo(meshinfo::EMeshType meshType, Tessellator& tessellator, int meshNum, const BlockPos& currentMin, bool generateCollision, bool castsShadow, UMaterialInterface* currentMaterial, int sectionIndex, const meshinfo::MeshInfo::SharedData& data)
	{
		meshinfo::MeshInfo meshInfo = CreateMeshInfo(meshType, meshNum, currentMin, data);

		meshInfo.meshSectionInfo.push_back(CreateMeshSection(tessellator, generateCollision, castsShadow, currentMaterial, sectionIndex));

		return meshInfo;
	}

	
}

TerrainGrid<short> createUnderfaceHeightRemovalThresholds(const std::vector<game::TilePtr>& tiles) {
	const auto maxThreshold = WalkableHeight::Max().toHeightThreshold().GetValue();

	TerrainGrid<short> out{ maxThreshold };

	for (game::TilePtr tile : tiles) {
		const auto& tileWalkablePlane = tile->tile().walkablePlane();
		const auto tileSize = tile->tile().size();
		const auto offset = tile->tilePlacement().blockPosTransform()(BlockPos::ZERO);

		for (const BlockPos pos : BlockCuboid::fromSize({ tileSize.x, 1, tileSize.z })) {
			const int x = pos.x + offset.x;
			const int z = pos.z + offset.z;
			const auto walkableHeight = tileWalkablePlane.get(pos.x, pos.z).withOffset(offset.y);
			out.set(x, z, walkableHeight.toHeightThreshold().Get(maxThreshold));
		}
	}
	return out;
}



void FLovikaTesselateTask::DoWork()
{
	if (doSimpleShadows)
	{
		DoShadowWork();
	}
	else
	{
		DoPrimaryWork();
	}
}

void ReduceAACollisionVolumesIntoMeshInfo(SimpleGeometryCollector &TerrainGeometryVolumeCollector, builder::meshinfo::MeshInfo &meshInfo)
{
	TerrainGeometryVolumeCollector.ReduceAACollisionBoxes();
	meshInfo.basicAxisAlignedCollisionBoxes = std::move(TerrainGeometryVolumeCollector.basicAxisAlignedCollisionBoxes);
	meshInfo.basicCollisionBoxes = std::move(TerrainGeometryVolumeCollector.basicCollisionBoxes);
}


void FLovikaTesselateTask::DoPrimaryWork()
{

#if SHOW_LOAD_TIMES
	double TaskTime = FPlatformTime::Seconds();
	double TimeVal = FPlatformTime::Seconds();
	double TimeStepVal = TimeVal;
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoPrimaryWork LOAD_TIME START %f \n"), TimeVal);
#endif


	using namespace builder;
	using namespace builder::meshinfo;

	MeshInfo::SharedData meshInfoSharedData {
		canHideBackFaces,
		&blockGraphicsPack,
	};

	//Chop up and tessellate world blocks for rendering
	{
		SimpleGeometryCollector TerrainGeometryVolumeCollector;
		SimpleGeometryCollector FillGeometryVolumeCollector;
		SimpleGeometryCollector LeavesGeometryVolumeCollector;
		SimpleGeometryCollector LavaGeometryVolumeCollector;

	#if EDITOR_SHADOW_TESTING
		ShadowQuadFaceTessellator tesselator;
	#else
		Tessellator tesselator;
	#endif

		BlockTessellator renderer(tesselator, blockGraphicsPack, region, RENDERCHUNK_SIDE);

		const auto underfaceThresholds = createUnderfaceHeightRemovalThresholds(subDungeonTiles);
		const auto underfaceFilter = [&](Vec3 p) {
			return p.y > underfaceThresholds.get(static_cast<int>(p.x), static_cast<int>(p.z));
		};

		renderer.setFaceFilter(underfaceFilter);
		renderer.setUnrealActor(owner);
		renderer.setSmoothLightingEnabled(useAmbientOcclusion);

		int nrMeshes = 0;
		int sectionIndex = 0;

		//Parse subtiles for collision culling regions
		std::vector<lovika::Region> NoCollisionRegionsAbove;
		std::vector<lovika::Region> NoCollisionRegionsBelow;

		std::vector<lovika::Region> NoBlockRegionsAbove;
		std::vector<lovika::Region> NoBlockRegionsBelow;

		for (auto&& tile : subDungeonTiles) 
		{
			if (abort) return;
			
			//below block cull regions, sort by highest first
			const auto& belowCullRegions = tile->tilePlacement().filterRegions(regionpredicates::isQuickCullBelow());

			if (belowCullRegions.size())
			{
				NoBlockRegionsBelow.reserve(NoBlockRegionsBelow.size() + belowCullRegions.size());

				for (const auto& belowRegion : belowCullRegions)
				{
					auto it = std::lower_bound(NoBlockRegionsBelow.begin(), NoBlockRegionsBelow.end(), belowRegion.y(), [](const lovika::Region& it, int val) {return (it.y() >= val); });
					NoBlockRegionsBelow.insert(it, belowRegion);
				}
			}

			//above cull regions, sort by lowest first
			const auto& aboveCullRegions = tile->tilePlacement().filterRegions(regionpredicates::isQuickCullAbove());

			if (aboveCullRegions.size())
			{
				NoBlockRegionsAbove.reserve(NoBlockRegionsAbove.size() + aboveCullRegions.size());

				for (const auto& aboveRegion : aboveCullRegions)
				{
					auto it = std::lower_bound(NoBlockRegionsAbove.begin(), NoBlockRegionsAbove.end(), aboveRegion.y(), [](const lovika::Region& it, int val) {return (it.y() <= val); });
					NoBlockRegionsAbove.insert(it, aboveRegion);
				}
			}


			//below cull regions, sort by highest first
			const auto& belowRegions = tile->tilePlacement().filterRegions(regionpredicates::isCollisionCullBelow());

			if (belowRegions.size())
			{
				NoCollisionRegionsBelow.reserve(NoCollisionRegionsBelow.size() + belowRegions.size());

				for (const auto& belowRegion : belowRegions)
				{
					auto it = std::lower_bound(NoCollisionRegionsBelow.begin(), NoCollisionRegionsBelow.end(), belowRegion.y(), [](const lovika::Region& it, int val) {return (it.y() >= val); });
					NoCollisionRegionsBelow.insert(it, belowRegion);
				}
			}

			//above cull regions, sort by lowest first
			const auto& aboveRegions = tile->tilePlacement().filterRegions(regionpredicates::isCollisionCullAbove());

			if (aboveRegions.size())
			{
				NoCollisionRegionsAbove.reserve(NoCollisionRegionsAbove.size() + aboveRegions.size());

				for (const auto& aboveRegion : aboveRegions)
				{
					auto it = std::lower_bound(NoCollisionRegionsAbove.begin(), NoCollisionRegionsAbove.end(), aboveRegion.y(), [](const lovika::Region& it, int val) {return (it.y() <= val); });
					NoCollisionRegionsAbove.insert(it, aboveRegion);
				}
			}
		}
		

		for (auto&& renderChunkPos : ChunkPos3Iterator<RENDERCHUNK_SIDE>::rangeIncl(subBounds)) {
			if(abort) return;
			
			const BlockCuboid renderChunkBounds = BlockCuboid(renderChunkPos.min(), renderChunkPos.maxExclusive()).intersection(subBounds);
			TArray< TArray<BlockInfo>, TFixedAllocator< BlockRenderLayer::_RENDERLAYER_COUNT > > infos;
			infos.SetNum(BlockRenderLayer::_RENDERLAYER_COUNT, false);

			BlockPos currentMin = renderChunkPos.min();

			if (NoBlockRegionsBelow.size() || NoBlockRegionsAbove.size())
			{
				for (auto&& bounds : region->blockVolumeHintsRender(renderChunkBounds)) {
					for (auto pos : bounds) {
						if (abort) return;
						FullBlock fullBlock = region->getBlockAndData(pos);
						const Block* block = Block::mBlocks[static_cast<int>(fullBlock.id)];
						if (block != nullptr && block != Block::mAir)
						{
							//check for discard
							bool bDoBlock = true;

							//try for below culling
							for (const BlockCuboid& belowRegion : NoBlockRegionsBelow)
							{
								if (pos.y < belowRegion.minInclusive.y)
								{
									if (belowRegion.containsXZ(pos))
									{
										bDoBlock = false;
										break;
									}
								}
								else
								{
									//regions sorted by depth, no further checking required
									break;
								}
							}
							if (bDoBlock)
							{
								//try above culling
								for (const BlockCuboid& aboveRegion : NoBlockRegionsAbove)
								{
									if (pos.y >= aboveRegion.minInclusive.y)
									{
										if (aboveRegion.containsXZ(pos))
										{
											bDoBlock = false;
											break;
										}
									}
									else
									{
										//regions sorted by depth, no further checking required
										break;
									}
								}
							}

							if(bDoBlock)
								infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, pos));

						}
					}
				}
			}
			else
			{
				for (auto&& bounds : region->blockVolumeHintsRender(renderChunkBounds)) {
					for (auto pos : bounds) {
						if (abort) return;
						FullBlock fullBlock = region->getBlockAndData(pos);
						const Block* block = Block::mBlocks[static_cast<int>(fullBlock.id)];
						if (block != nullptr && block != Block::mAir)
						{
							infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, pos));
						}
					}
				}
			}

			bool bIsChunkPureFill = (infos[BlockRenderLayer::RENDERLAYER_OPAQUE_FILL].Num() > 0);

			if(bIsChunkPureFill)
			{
				//lets see if this mesh contains anything other than fill
				for (int renderLayer = BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED; renderLayer < BlockRenderLayer::_RENDERLAYER_COUNT; ++renderLayer)
				{
					if(renderLayer == BlockRenderLayer::RENDERLAYER_OPAQUE_FILL)
						continue;
					
					if (infos[renderLayer].Num() > 0)
					{
						bIsChunkPureFill = false;
						break;
					}
				}
			}

			//Create Lava collision nav modifier volumes
			if (infos[BlockRenderLayer::RENDERLAYER_LAVA].Num() > 0)
			{
				auto& LavaInfos = infos[BlockRenderLayer::RENDERLAYER_LAVA];

				LavaGeometryVolumeCollector.clear();

				for (auto info : LavaInfos)
				{
					if (abort) return;
					BlockPos CheckBlock = info.pos;
					FullBlock this_fullBlock = region->getBlockAndData(CheckBlock);
					const Block* this_block = Block::mBlocks[static_cast<int>(this_fullBlock.id)];

					FullBlock up_fullBlock = region->getBlockAndData(CheckBlock.above());
					const Block* up_block = Block::mBlocks[static_cast<int>(up_fullBlock.id)];

					//only do blocks that are ground level
					if (up_block == Block::mAir)
					{
						//search adjacent blocks to see if we are a border block
						const int iCheckBlockCount = 4;
						const BlockPos CheckBlocks[iCheckBlockCount] = {
							BlockPos(CheckBlock.x,		CheckBlock.y,	CheckBlock.z + 1),
							BlockPos(CheckBlock.x - 1,	CheckBlock.y,	CheckBlock.z),
							BlockPos(CheckBlock.x + 1,	CheckBlock.y,	CheckBlock.z),
							BlockPos(CheckBlock.x,		CheckBlock.y,	CheckBlock.z - 1)
						};

						for (int j(0); j < iCheckBlockCount; ++j)
						{
							FullBlock check_fullBlock = region->getBlockAndData(CheckBlocks[j]);
							const Block* check_block = Block::mBlocks[static_cast<int>(check_fullBlock.id)];

							if (check_block != Block::mAir && check_block != this_block)
							{
								const FVector CornerPos(info.pos.x, info.pos.z, info.pos.y);//yz flip here
								const FVector Extent(1.0f, 1.0f, 2.0f);
								LavaGeometryVolumeCollector.AddBasicAxisAlignedCollisionVolume(CornerPos, Extent);
								break;
							}
						}
					}
				}
			}

			

			// D11.DH put this here to allow use of terrain meshes with multiple sections
			meshinfo::MeshInfo terrainMeshInfo = CreateMeshInfo(EMeshType::E_MESHTYPE_TERRAIN, nrMeshes, currentMin, meshInfoSharedData);

			TerrainGeometryVolumeCollector.clear();
			tesselator.SetSimpleGeometryVolumeCollector(nullptr);

			sectionIndex = 0;
			for (int renderLayer = BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED; renderLayer < BlockRenderLayer::_RENDERLAYER_COUNT; ++renderLayer)
			{

				if(abort) return;
				const TArray<BlockInfo>& blocks = infos[renderLayer]; //blockInfo.Value;

				if (blocks.Num() == 0)
				{
					continue;
				}

				renderer.setRenderLayer(renderLayer);
				renderer.setForceOpaque(renderLayer == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
				renderer.resetCache(currentMin, *region);
				renderer.clear();

				SimpleGeometryCollector* pCurrentCollector = nullptr;
				
				switch (renderLayer) {
				case BlockRenderLayer::RENDERLAYER_LAVA:
				case BlockRenderLayer::RENDERLAYER_WATER:				
				case BlockRenderLayer::RENDERLAYER_ALPHATEST:
				case BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST:
				case BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED:
				case BlockRenderLayer::RENDERLAYER_BLEND:
				case BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST:
				case BlockRenderLayer::RENDERLAYER_ALPHATEST_FOILAGE:
				case BlockRenderLayer::RENDERLAYER_TORCH:
				case BlockRenderLayer::RENDERLAYER_FIRE:
				case BlockRenderLayer::RENDERLAYER_VOID:
					break;
				case BlockRenderLayer::RENDERLAYER_LEAVES:
					pCurrentCollector = &LeavesGeometryVolumeCollector;
					LeavesGeometryVolumeCollector.clear();
					break;
				case BlockRenderLayer::RENDERLAYER_OPAQUE_FILL:
					pCurrentCollector = ((bIsChunkPureFill) ? nullptr : &FillGeometryVolumeCollector);
					FillGeometryVolumeCollector.clear();
					break;

				default:
					pCurrentCollector = &TerrainGeometryVolumeCollector;
					break;
				}

				tesselator.SetSimpleGeometryVolumeCollector(pCurrentCollector);

#if EDITOR_SHADOW_TESTING
				// Testing Shadow Block gen in editor
				for (auto& bb : blocks)
				{
					const Block* block = Block::mBlocks[static_cast<int>(bb.fullBlock.id)];
					renderer.tessellateInWorld(*block, bb.pos, bb.fullBlock.data, true, true);
				}

				tesselator.ReduceBlockFaces(); 
				tesselator.BuildBlocksFromFaces();
#else
				
				// Tesselate normal render blocks

				if(pCurrentCollector && (NoCollisionRegionsBelow.size() || NoCollisionRegionsAbove.size()))
				{
					for (auto& bb : blocks)
					{
						const Block* block = Block::mBlocks[static_cast<int>(bb.fullBlock.id)];

						bool bDoCollision = true;

						//try for below culling
						for (const BlockCuboid& belowRegion : NoCollisionRegionsBelow)
						{
							if (bb.pos.y < belowRegion.minInclusive.y)
							{
								if (belowRegion.containsXZ(bb.pos))
								{
									bDoCollision = false;
									break;
								}
							}
							else
							{
								//regions sorted by depth, no further checking required
								break;
							}
						}
						if (bDoCollision)
						{
							//try above culling
							for (const BlockCuboid& aboveRegion : NoCollisionRegionsAbove)
							{
								if (bb.pos.y >= aboveRegion.minInclusive.y)
								{
									if (aboveRegion.containsXZ(bb.pos))
									{
										bDoCollision = false;
										break;
									}
								}
								else
								{
									//regions sorted by depth, no further checking required
									break;
								}
							}
						}
						
						renderer.tessellateInWorld(*block, bb.pos, bb.fullBlock.data, true, false, bDoCollision);
					}
				}
				else
				{
					//no geometry collisions collected or no areas to cull, do basic
					for (auto& bb : blocks)
					{
						const Block* block = Block::mBlocks[static_cast<int>(bb.fullBlock.id)];
						renderer.tessellateInWorld(*block, bb.pos, bb.fullBlock.data);
					}
				}
#endif

				if (tesselator.vertices.Num() > 0 ) 
				{
					bool generateCollision = (CVarSimpleCollisionGeometry.GetValueOnAnyThread() == 0);
#if DO_SIMPLE_SHADOW_MESHES
					bool castsShadow = false;
#else
					bool castsShadow = Block::RenderLayerCastsShadow((BlockRenderLayer)renderLayer);
#endif
					UMaterialInterface* currentMaterial = blockGraphicsPack.atlasMaterialInstanceOpaque.Get();

					switch (renderLayer) {
					case BlockRenderLayer::RENDERLAYER_WATER:
						currentMaterial = baseBuilder->materialWater;
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_LAVA:
						currentMaterial = baseBuilder->materialEmissive;
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE:
						currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlpha.Get();
						break;
					case BlockRenderLayer::RENDERLAYER_ALPHATEST:
					case BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST:
					case BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED:
					case BlockRenderLayer::RENDERLAYER_BLEND:
					case BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST:
						currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlpha.Get();
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_OPAQUE_ADDITIONAL_ZEROALPHA_GEOMETRY:
						currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlpha.Get();
						break;
					case BlockRenderLayer::RENDERLAYER_ALPHATEST_FOILAGE:
						currentMaterial = blockGraphicsPack.atlasMaterialInstanceAlphaFoilage.Get();
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_TORCH:
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_FIRE:
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_LEAVES:
						currentMaterial = blockGraphicsPack.atlasMaterialInstanceLeaves.Get();
						break;
					case BlockRenderLayer::RENDERLAYER_OPAQUE_FILL:
						generateCollision = false;
						break;
					case BlockRenderLayer::RENDERLAYER_VOID:
						currentMaterial = baseBuilder->materialVoid;
						generateCollision = false;
						break;
					default:
						break;
					}
					
					if (renderLayer == RENDERLAYER_WATER) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_WATER, tesselator, nrMeshes, currentMin, generateCollision, castsShadow, currentMaterial, 0, meshInfoSharedData);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					if (renderLayer == RENDERLAYER_LEAVES || renderLayer == RENDERLAYER_ALPHATEST_FOILAGE) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_LEAF, tesselator, nrMeshes, currentMin, generateCollision, castsShadow, currentMaterial, 0, meshInfoSharedData);
						ReduceAACollisionVolumesIntoMeshInfo(LeavesGeometryVolumeCollector, Meshinfo);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					if (renderLayer == RENDERLAYER_LAVA) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_LAVA, tesselator, nrMeshes, currentMin, generateCollision, castsShadow, currentMaterial, 0, meshInfoSharedData);
						ReduceAACollisionVolumesIntoMeshInfo(LavaGeometryVolumeCollector, Meshinfo);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					if (renderLayer == RENDERLAYER_OPAQUE_FILL)
					{
						nrMeshes++;						

						//Pure Fill Chunks can have full AABB
						if (bIsChunkPureFill)
						{
							tesselator.SetSimpleGeometryVolumeCollector(&FillGeometryVolumeCollector);
							FillGeometryVolumeCollector.clear();
							tesselator.CreateFullAxisAlignedBoundingBoxCollisionVolume();
						}

						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_TERRAIN_FILL, tesselator, nrMeshes, currentMin, generateCollision, castsShadow, currentMaterial, 0, meshInfoSharedData);
						ReduceAACollisionVolumesIntoMeshInfo(FillGeometryVolumeCollector, Meshinfo);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						FillGeometryVolumeCollector.clear();
						continue;
					}

					if (renderLayer == RENDERLAYER_VOID) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_TERRAIN, tesselator, nrMeshes, currentMin, generateCollision, castsShadow, currentMaterial, 0, meshInfoSharedData);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					terrainMeshInfo.meshSectionInfo.push_back(CreateMeshSection(tesselator, generateCollision, castsShadow, currentMaterial, sectionIndex++));

				}
			}
			// D11.DH: Ensure that meshes from empty chunks are not added
			if (terrainMeshInfo.meshSectionInfo.size() > 0 || TerrainGeometryVolumeCollector.hasBasicCollisionVolumes())
			{
				++nrMeshes;
				ReduceAACollisionVolumesIntoMeshInfo(TerrainGeometryVolumeCollector, terrainMeshInfo);
				baseBuilder->AddTesselatedMeshInfo(terrainMeshInfo);
			}

			TerrainGeometryVolumeCollector.clear();
		}

#if SHOW_LOAD_TIMES
		TimeVal = FPlatformTime::Seconds();
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoPrimaryWork LOAD_TIME Primary Chop %f total %f \n"), TimeVal - TimeStepVal, TimeVal - TaskTime);
		TimeStepVal = TimeVal;
#endif
		
		{
			TerrainGeometryVolumeCollector.clear();
			tesselator.SetSimpleGeometryVolumeCollector(&TerrainGeometryVolumeCollector);

			//  Invisible walls
			{
				BoundaryTessellator boundaryTessellator{ tesselator };
				
				const auto tesselateBoundaries = [&, this](std::vector<meshinfo::MeshSectionInfo>& target, const std::vector<BlockCuboid>& boundaries) {
					if (boundaries.empty()) {
						return;
					}

					boundaryTessellator.clear();
					for (auto&& boundary : boundaries) {
						if (abort) {
							return;
						}
						boundaryTessellator.tessellateInWorld(boundary);
					}
					
					UMaterialInterface*  material = baseBuilder->materialSolid;
					target.push_back(CreateMeshSection(tesselator, false, false, material, ++sectionIndex));
				};

				{
					MeshInfo::SharedData wallMeshInfoSharedData{
					canHideBackFaces,
					&blockGraphicsPack,
					};
					
					boundaryTessellator.SetProduceMesh(ShowInvisibleWalls >= 1);
					boundaryTessellator.SetProduceSimpleCollision(true);
					
					for (auto&& tile : subDungeonTiles) {
						if (abort) return;

						if(tile->tilePlacement().boundaries().size() == 0)
							continue;

						const int32 iGridSize = CVarBoundryGridSize.GetValueOnAnyThread();

						BlockCuboid TileBounds = tile->bounds();
						BlockPos TileSize = TileBounds.size();
						BlockPos TileQSize(TileSize.x / iGridSize, TileSize.y, TileSize.z / iGridSize);

						//Split invisible wall geometry into grid rather than colliding with one big actor component
						for (int32 iZ = 0; iZ < iGridSize; ++iZ)
						{
							for (int32 iX = 0; iX < iGridSize; ++iX)
							{
								const BlockCuboid GridArea = BlockCuboid::fromPositionAndSize(TileBounds.minInclusive + BlockPos(TileQSize.x * iX, 0, TileQSize.z * iZ), TileQSize);
								auto BoundryList = algo::map_if_vector(tile->tilePlacement().boundaries(), RETLAMBDA(it), RETLAMBDA(GridArea.containsXZ(it.position)));
								
								if (BoundryList.size() > 0)
								{
									// Regular invisible walls (generated by editor)
									wallMeshInfoSharedData.customName = tile->tile().id() + std::to_string(iZ) + std::to_string(iX);
									meshinfo::MeshInfo meshInfo = CreateMeshInfo(EMeshType::E_MESHTYPE_INVISIBLE_WALL, ++nrMeshes, {}, wallMeshInfoSharedData);
									TerrainGeometryVolumeCollector.clear();
									std::vector<BlockCuboid> BoundryListCuboid = algo::map_vector(BoundryList, RETLAMBDA(it.toBlockCuboid()));
									boundaryTessellator.SetColor(1.0f, 0.0f, 0.0f, InvisibleWallsAlpha);	//Blue?
									tesselateBoundaries(meshInfo.meshSectionInfo, BoundryListCuboid);
									ReduceAACollisionVolumesIntoMeshInfo(TerrainGeometryVolumeCollector, meshInfo);
									baseBuilder->AddTesselatedMeshInfo(meshInfo);
								}
							}
						}
					}
					
				}

#if SHOW_LOAD_TIMES
				TimeVal = FPlatformTime::Seconds();
				FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoPrimaryWork LOAD_TIME Regular invisible walls Chop %f total %f \n"), TimeVal - TimeStepVal, TimeVal - TaskTime); TimeStepVal = TimeVal;
#endif

				{
					MeshInfo::SharedData killZoneMeshInfoSharedData{
					canHideBackFaces,
					&blockGraphicsPack,
					};
					// Invisible and conditional walls (killzones + outside tile blockers)
					static constexpr int DefaultBoundaryHeight = 40;

					boundaryTessellator.SetProduceMesh(ShowKillZones >= 1);
					boundaryTessellator.SetProduceSimpleCollision(true);

					for (auto&& tile : subDungeonTiles) {
						if (abort) return;

						killZoneMeshInfoSharedData.customName = tile->tile().id();

						meshinfo::MeshInfo meshInfo = CreateMeshInfo(EMeshType::E_MESHTYPE_INVISIBLE_KILLZONE, ++nrMeshes, {}, killZoneMeshInfoSharedData);
						TerrainGeometryVolumeCollector.clear();

						auto boundaries = algo::copy_if_map_vector(tile->tilePlacement().regionPlane(),
							RETLAMBDA(TerrainCell(it.value).isKillzone()),
							RETLAMBDA(Boundary(it.position, DefaultBoundaryHeight).toBlockCuboid()));

						if (!tile->dungeon().def().deathOutsideTile) {
							const auto outsideTileBoundaryBlocks = algo::copy_if_map_vector(rimBlocksXzOutsideOf(tile->bounds()),
								RETLAMBDA(!tiles.getTile(it)),
								RETLAMBDA(Boundary(it, DefaultBoundaryHeight)));

							const auto outsideTileMergedBoundaries = mergeBoundaries(outsideTileBoundaryBlocks);
							boundaries.insert(boundaries.end(), outsideTileMergedBoundaries.begin(), outsideTileMergedBoundaries.end());
						}
						boundaryTessellator.SetColor(0.0f, 0.0f, 1.0f, KillZoneAlpha);	//RED?
						tesselateBoundaries(meshInfo.meshSectionInfo, boundaries);

						ReduceAACollisionVolumesIntoMeshInfo(TerrainGeometryVolumeCollector, meshInfo);
						baseBuilder->AddTesselatedMeshInfo(meshInfo);
					}
					
					TerrainGeometryVolumeCollector.clear();
#if SHOW_LOAD_TIMES
					TimeVal = FPlatformTime::Seconds();
					FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoPrimaryWork LOAD_TIME Invisible and conditional walls Chop %f total %f \n"), TimeVal - TimeStepVal, TimeVal - TaskTime);
					TimeStepVal = TimeVal;
#endif

				}
			}

		}

		UE_LOG(LogTemp, Warning, TEXT("Total nr meshes: %d"), nrMeshes);
	}

#if SHOW_LOAD_TIMES
	TimeVal = FPlatformTime::Seconds();
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoPrimaryWork LOAD_TIME %f total %f \n"), TimeVal - TimeStepVal, TimeVal - TaskTime);
	TimeStepVal = TimeVal;
#endif

}



void FLovikaTesselateTask::DoShadowWork()
{

#if DO_SIMPLE_SHADOW_MESHES

#if SHOW_LOAD_TIMES
	double TaskTime = FPlatformTime::Seconds();
	double TimeVal = FPlatformTime::Seconds();
	double TimeStepVal = TimeVal;
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoShadowWork LOAD_TIME START %f \n"), TimeVal);
#endif

	using namespace builder;
	using namespace builder::meshinfo;
	
	MeshInfo::SharedData meshInfoSharedData{
		canHideBackFaces,
		&blockGraphicsPack,
	};

	const BlockGraphics& dirtBlockGraphics = blockGraphicsPack.get(Block::mDirt->getId());

	ShadowQuadFaceTessellator ShadowTesselator;

	BlockTessellator renderer(ShadowTesselator, blockGraphicsPack, region, FULL_SCREEN_SHADOW_RENDERCHUNK_SIDE);

	

	renderer.setUnrealActor(owner);
	renderer.setSmoothLightingEnabled(useAmbientOcclusion);

	int nrMeshes = 0;
	int sectionIndex = 0;


	//Parse subtiles for shadow culling regions
	std::vector<lovika::Region> NoShadowRegionsAbove;
	std::vector<lovika::Region> NoShadowRegionsBelow;
	

	for (auto&& tile : subDungeonTiles)
	{
		if (abort) return;

		//below cull regions, sort by highest first
		const auto& belowRegions = tile->tilePlacement().filterRegions(regionpredicates::isShadowCullBelow());

		if (belowRegions.size())
		{
			NoShadowRegionsBelow.reserve(NoShadowRegionsBelow.size() + belowRegions.size());

			for (const auto& belowRegion : belowRegions)
			{
				auto it = std::lower_bound(NoShadowRegionsBelow.begin(), NoShadowRegionsBelow.end(), belowRegion.y(), [](const lovika::Region& it, int val) {return (it.y() >= val); });
				NoShadowRegionsBelow.insert(it, belowRegion);
			}
		}

		//above cull regions, sort by lowest first
		const auto& aboveRegions = tile->tilePlacement().filterRegions(regionpredicates::isShadowCullAbove());

		if (aboveRegions.size())
		{
			NoShadowRegionsAbove.reserve(NoShadowRegionsAbove.size() + aboveRegions.size());

			for (const auto& aboveRegion : aboveRegions)
			{
				auto it = std::lower_bound(NoShadowRegionsAbove.begin(), NoShadowRegionsAbove.end(), aboveRegion.y(), [](const lovika::Region& it, int val) {return (it.y() <= val); });
				NoShadowRegionsAbove.insert(it, aboveRegion);
			}
		}
	}

	//Chop up and tessellate world blocks for basic geometry rendering
	// Shadow Meshes created here

	for (auto&& renderChunkPos : ChunkPos3Iterator<FULL_SCREEN_SHADOW_RENDERCHUNK_SIDE>::rangeIncl(subBounds))
	{
		if (abort) return;
		const BlockCuboid renderChunkBounds = BlockCuboid(renderChunkPos.min(), renderChunkPos.maxExclusive()).intersection(subBounds);
		SHOW_LOAD_TIME_CALL(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### renderChunkBounds %s volume %d ##################\n"), UTF8_TO_TCHAR(renderChunkBounds.toString().c_str()), renderChunkBounds.volume()));
		
		TArray< TArray<BlockInfo>, TFixedAllocator< BlockRenderLayer::_RENDERLAYER_COUNT > > infos;

		infos.SetNum(BlockRenderLayer::_RENDERLAYER_COUNT, false);

		BlockPos currentMin = renderChunkPos.min();
		
		if (NoShadowRegionsAbove.size() || NoShadowRegionsBelow.size())
		{
			SHOW_LOAD_TIME_CALL(double TestTime = FPlatformTime::Seconds());

			for (auto&& bounds : region->blockVolumeHintsShadowed(renderChunkBounds)) {
				SHOW_LOAD_TIME_CALL(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### blockVolumeHints bounds %s volume %d ##################\n"), UTF8_TO_TCHAR(bounds.toString().c_str()), bounds.volume()));
				for (auto pos : bounds) {
					FullBlock fullBlock = region->getBlockAndData(pos);
					const Block* block = Block::mBlocks[static_cast<int>(fullBlock.id)];
					if (block != nullptr && block != Block::mAir && block->getRenderLayerCastsShadow())
					{
						bool bDoShadow = true;
						//try for below culling
						for (const BlockCuboid& belowRegion : NoShadowRegionsBelow)
						{
							if (pos.y < belowRegion.minInclusive.y)
							{
								if (belowRegion.containsXZ(pos))
								{
									bDoShadow = false;
									break;
								}
							}
							else
							{
								//regions sorted by depth, no further checking required
								break;
							}
						}
						if (bDoShadow)
						{
							//try above culling
							for (const BlockCuboid& aboveRegion : NoShadowRegionsAbove)
							{
								if (pos.y >= aboveRegion.minInclusive.y)
								{
									if (aboveRegion.containsXZ(pos))
									{
										bDoShadow = false;
										break;
									}
								}
								else
								{
									//regions sorted by depth, no further checking required
									break;
								}
							}
						}

						if (bDoShadow)
						{
							infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, pos));
						}
					}
				}
			}

			SHOW_LOAD_TIME_CALL(double EndTestTime = FPlatformTime::Seconds());
			SHOW_LOAD_TIME_CALL(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### ShadowRegions blockVolumeHints  %f Ms (%f s) ##################\n"), (EndTestTime - TestTime)*1000.0, (EndTestTime - TestTime)));


		}
		else
		{
			SHOW_LOAD_TIME_CALL(double TestTime = FPlatformTime::Seconds());

			for (auto&& bounds : region->blockVolumeHintsShadowed(renderChunkBounds)) {
				for (auto pos : bounds) {
					FullBlock fullBlock = region->getBlockAndData(pos);
					const Block* block = Block::mBlocks[static_cast<int>(fullBlock.id)];
					if (block != nullptr && block != Block::mAir && block->getRenderLayerCastsShadow())
					{
						infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, pos));
					}
				}
			}

			SHOW_LOAD_TIME_CALL(double EndTestTime = FPlatformTime::Seconds());
			SHOW_LOAD_TIME_CALL(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### blockVolumeHints  %f Ms (%f s) ##################\n"), (EndTestTime - TestTime)*1000.0, (EndTestTime - TestTime)));

		}
		
		// D11.DH put this here to allow use of terrain shadow meshes with multiple sections
		meshinfo::MeshInfo terrainShadowMeshInfo = CreateMeshInfo(EMeshType::E_MESHTYPE_TERRAIN_SHADOW, ++nrMeshes, currentMin, meshInfoSharedData);
		sectionIndex = 0;

		for (int renderLayer = BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED; renderLayer < BlockRenderLayer::_RENDERLAYER_COUNT; ++renderLayer)
		{
			if (abort) return;
			const TArray<BlockInfo>& blocks = infos[renderLayer]; //blockInfo.Value;

			if (blocks.Num() == 0 || !Block::RenderLayerCastsShadow((BlockRenderLayer)renderLayer)) //no blocks or shadow layer?
			{
				continue;
			}

			UMaterialInterface* currentMaterial = baseBuilder->VoxelMatFSShadowMaterialInstanceOpaque;

			switch (renderLayer)
			{
			case BlockRenderLayer::RENDERLAYER_WATER:
				currentMaterial = baseBuilder->materialWater;
				break;
			case BlockRenderLayer::RENDERLAYER_LAVA:
				currentMaterial = baseBuilder->materialEmissive;
				break;
			case BlockRenderLayer::RENDERLAYER_LEAVES:
				currentMaterial = blockGraphicsPack.atlasMaterialInstanceLeaves.Get();
				break;

			default:
				break;
			}
			
			{
				{
					renderer.setRenderLayer(renderLayer);
					renderer.setForceOpaque(renderLayer == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
					renderer.resetCache(currentMin, *region);
					renderer.clear();

					{
						SHOW_LOAD_TIME_CALL(double TestTime = FPlatformTime::Seconds());

						for (auto& bb : blocks)
						{
							const Block* block = Block::mBlocks[static_cast<int>(bb.fullBlock.id)];
							renderer.tessellateInWorldShadow(*block, bb.pos, bb.fullBlock.data, dirtBlockGraphics,true, false);
						}
						
						SHOW_LOAD_TIME_CALL(double EndTestTime = FPlatformTime::Seconds());
						SHOW_LOAD_TIME_CALL(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### Shadow tessellateInWorld blocks (%d)  %f Ms (%f s) ##################\n"), blocks.Num(), (EndTestTime - TestTime)*1000.0, (EndTestTime - TestTime)));
					}
					
					ShadowTesselator.ReduceBlockFaces();
					ShadowTesselator.BuildBlocksFromFaces();
				}


				if (ShadowTesselator.vertices.Num() > 0)
				{

					if (renderLayer == RENDERLAYER_WATER) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_WATER_SHADOW, ShadowTesselator, nrMeshes, currentMin, false, true, currentMaterial, 0, meshInfoSharedData);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					if (renderLayer == RENDERLAYER_LEAVES || renderLayer == RENDERLAYER_ALPHATEST_FOILAGE) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_LEAF_SHADOW, ShadowTesselator, nrMeshes, currentMin, false, true, currentMaterial, 0, meshInfoSharedData);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					if (renderLayer == RENDERLAYER_LAVA) {
						nrMeshes++;
						auto Meshinfo = CreateMeshInfo(EMeshType::E_MESHTYPE_LAVA_SHADOW, ShadowTesselator, nrMeshes, currentMin, false, true, currentMaterial, 0, meshInfoSharedData);
						baseBuilder->AddTesselatedMeshInfo(Meshinfo);
						continue;
					}

					terrainShadowMeshInfo.meshSectionInfo.push_back(CreateMeshSection(ShadowTesselator, false, true, currentMaterial, sectionIndex++));
				}
			}
		}

		//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("AddTesselatedMeshInfo %d Sections : %d\n"), terrainShadowMeshInfo.meshNum, terrainShadowMeshInfo.meshSectionInfo.size());
		if (terrainShadowMeshInfo.meshSectionInfo.size() > 0)
		{
			baseBuilder->AddTesselatedMeshInfo(terrainShadowMeshInfo);
		}
	}

#if SHOW_LOAD_TIMES
	TimeVal = FPlatformTime::Seconds();
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoShadowWork LOAD_TIME Shadow Chop %f total %f \n"), TimeVal - TimeStepVal, TimeVal - TaskTime); TimeStepVal = TimeVal;
#endif

	UE_LOG(LogTemp, Warning, TEXT("Total nr meshes: %d"), nrMeshes);


#if SHOW_LOAD_TIMES
	TimeVal = FPlatformTime::Seconds();
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#FLovikaTesselateTask::DoShadowWork LOAD_TIME %f total %f \n"), TimeVal - TimeStepVal, TimeVal - TaskTime);
	TimeStepVal = TimeVal;
#endif


#endif
}


FLovikaTesselateTask::FLovikaTesselateTask(
	ULovikaGeneratorBuilder* builder, 
	AGameBP* gameBp, 
	AActor* actor,  
	ChunkBlockSource* chunk, 
	const game::Tiles& tileData, 
	bool useAo, 
	bool doShadow,
	const bool& abortFlag, 
	const BlockGraphicsPack& blockgraphicsPack, 
	std::vector<game::TilePtr>& subTiles, 
	const BlockCuboid& bounds,
	bool canHideBackFaces
)
	: baseBuilder(builder)
	, game(gameBp)
	, owner(actor)
	, region(chunk)
	, tiles(tileData)
	, useAmbientOcclusion(useAo)
	, doSimpleShadows(doShadow)
	, abort(abortFlag)
	, blockGraphicsPack(blockgraphicsPack)
	, subDungeonTiles(subTiles)
	, subBounds(bounds)
	, canHideBackFaces(canHideBackFaces) {
}


ULovikaGeneratorBuilder::ULovikaGeneratorBuilder()
	:ULovikaBaseBuilder()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	mTotalTris = 0;
	mTotalShadowTris = 0;
	mSimpleCollisionVolumes = 0;
}

#if SHOW_LOAD_TIMES

double gBuilderLoadTime = -1.0;
double gbHaveMeshGenTasksCompleted = -1.0;

double gbmMeshGenJobs = -1.0;
double gbmTesselatedMeshQueue = -1.0;
double gbmUnparsedRuntimeMeshes = -1.0;

#define GEN_TEST_TIME(x) { if(x && g##x <= 0.0){g##x = FPlatformTime::Seconds();} }
#define GEN_TEST_TIME_INIT(x) { g##x = -1.0; }
#define GEN_TEST_TIME_PRINT(x) { double val = (g##x - gBuilderLoadTime); FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n## ULovikaGeneratorBuilder "#x" LOAD_TIME : %f (%f s)\n"), g##x, val); }

#endif


void ULovikaGeneratorBuilder::build(bool useAmbientOcclusion, ChunkBlockSource* region, const game::Tiles& tiles, bool canHideBackFaces) {
	
#if SHOW_LOAD_TIMES
	gBuilderLoadTime = FPlatformTime::Seconds();
	GEN_TEST_TIME_PRINT(BuilderLoadTime);
	GEN_TEST_TIME_INIT(bHaveMeshGenTasksCompleted);
	GEN_TEST_TIME_INIT(bmMeshGenJobs);
	GEN_TEST_TIME_INIT(bmTesselatedMeshQueue);
	GEN_TEST_TIME_INIT(bmUnparsedRuntimeMeshes);
#endif
	mTotalTris = 0;
	mTotalShadowTris = 0;
	mSimpleCollisionVolumes = 0;

	// D11.DH
	// Async loading of Level meshes
	// Mesh creation needs to be on game thread because doing that async doesn't work
	// Tessellate will be able to be done async as that doesn't seem to cause any issues

	//Ensure we are not running any teselation currently.
	AbortAndWait();
	// should be a pretty cheap call so can be done here
	_clearMeshes();

	AActor* owner = GetOwner();
	AGameBP* game = actorquery::getFirstActor<AGameBP>(owner->GetWorld());
	for (auto subdungeon : tiles.getSubDungeonInfos()) 
	{
		//D11.SC Dungeon subbounds need to be oversized to take into account the boundry fill, as this does not get included in the tiles bounds
		BlockPos GrowSize(postprocess::worldfill::NumBorderBlocks, 0, postprocess::worldfill::NumBorderBlocks);
		BlockCuboid subdungeonBounds = growed(subdungeon.bounds, GrowSize);

		mMeshGenJobs.emplace_back(std::make_unique<FAsyncTask<FLovikaTesselateTask>>(this, game, owner, region, tiles, useAmbientOcclusion, false, _abortFlag, subdungeon.dungeon.blockGraphicsPack(GetWorld()), subdungeon.tiles, subdungeonBounds, canHideBackFaces));
#if DO_SIMPLE_SHADOW_MESHES
		mMeshGenJobs.emplace_back(std::make_unique<FAsyncTask<FLovikaTesselateTask>>(this, game, owner, region, tiles, useAmbientOcclusion, true, _abortFlag, subdungeon.dungeon.blockGraphicsPack(GetWorld()), subdungeon.tiles, subdungeonBounds, canHideBackFaces));
#endif

	}

	//Kick all jobs
	for(auto& job : mMeshGenJobs)
	{
		job->StartBackgroundTask();
		//job->StartSynchronousTask(); //TEMP!
	}

	SetComponentTickEnabled(true);
}

bool ULovikaGeneratorBuilder::HaveMeshGenTasksCompleted() const
{
	if (mMeshGenJobs.empty())
	{
#if SHOW_LOAD_TIMES
		bool bmTesselatedMeshQueue = mTesselatedMeshQueue.IsEmpty(); GEN_TEST_TIME(bmTesselatedMeshQueue);
		bool bmUnparsedRuntimeMeshes = (mUnparsedRuntimeMeshes.Num() == 0); GEN_TEST_TIME(bmUnparsedRuntimeMeshes);

		return (bmTesselatedMeshQueue && bmUnparsedRuntimeMeshes);
#else
		return (mTesselatedMeshQueue.IsEmpty() && mUnparsedRuntimeMeshes.Num() == 0);
#endif

	}

	//IsWorkDone is non-blocking and will simply check if the computation is complete. The task may still not have been dequeued and marked as finished at this point.
	//IsDone will block if IsWorkDone is true, and wait for completion, but since we can have multiple jobs running in this design, we should not block until all have
	//completed their work.
	if(!std::all_of(mMeshGenJobs.begin(), mMeshGenJobs.end(), [] (const auto& v) { return v->IsWorkDone();})) return false;

	//Finally, all work is done and we block until all tasks have been finished.
	if (std::all_of(mMeshGenJobs.begin(), mMeshGenJobs.end(), [](const auto& v) { return v->IsDone(); }))
	{

#if SHOW_LOAD_TIMES
		bool bmMeshGenJobs = true; GEN_TEST_TIME(bmMeshGenJobs);

		//check all meshes have been processed
		//return (mTesselatedMeshQueue.IsEmpty() && mNavigationMeshQueue.IsEmpty() && mUnparsedRuntimeMeshes.Num() == 0);
		bool bmTesselatedMeshQueue = mTesselatedMeshQueue.IsEmpty(); GEN_TEST_TIME(bmTesselatedMeshQueue);
		bool bmUnparsedRuntimeMeshes = (mUnparsedRuntimeMeshes.Num() == 0); GEN_TEST_TIME(bmUnparsedRuntimeMeshes);

		return (bmTesselatedMeshQueue && bmUnparsedRuntimeMeshes);
#else
		return (mTesselatedMeshQueue.IsEmpty() && mUnparsedRuntimeMeshes.Num() == 0);
#endif

	}

	return false;
}

void  ULovikaGeneratorBuilder::SetCreatedWorldMeshes()
{
	if(!HaveMeshGenTasksCompleted())
	{
		UE_LOG(LogTemp, Error, TEXT("Create world meshes called when mesh generation has not completed, terminating."));
		return;
	}

	mMeshesGenerated = true;
	mMeshGenJobs.clear();
}

void ULovikaGeneratorBuilder::DumpExcessMeshData()
{
	//We arent interested in the mesh data stored within the runtime mesh once its been used to create the Physics and Graphics Data as it never gets changed
	AActor* owner = GetOwner();
	auto meshes = owner->GetComponentsByClass(URuntimeMeshComponent::StaticClass());
	for (auto component : meshes)
	{
		if (const auto sceneComponent = Cast<URuntimeMeshComponent>(component))
		{
			sceneComponent->ClearMeshDataBuffers();
		}
	}
}

void ULovikaGeneratorBuilder::BeginDestroy()
{
	AbortAndWait();
	Super::BeginDestroy();
}

void ULovikaGeneratorBuilder::AddTesselatedMeshInfo(builder::meshinfo::MeshInfo& sourceMeshInfo)
{
	if (sourceMeshInfo.meshSectionInfo.size() > 0) //dont try to add meshes with no sections
		mTesselatedMeshQueue.Enqueue(std::move(sourceMeshInfo));
}


void ULovikaGeneratorBuilder::ProcessGeneratedWorldMeshes()
{
	using namespace builder::meshinfo;

	//Dump any excess mesh data from Completed runtime meshes
	for (auto Iter = mUnparsedRuntimeMeshes.CreateIterator(); Iter; ++Iter)
	{
		auto sceneComponent = *Iter;
		if (!sceneComponent.IsValid())
		{
			Iter.RemoveCurrent();
		}
		else
		{
			const bool needsRenderState = sceneComponent->bVisible || sceneComponent->bCastHiddenShadow;
			const bool isRenderStatePending = needsRenderState && (!sceneComponent->IsRenderStateCreated() || !sceneComponent->SceneProxy);
			const bool isPhysicsStatePending = sceneComponent->GetCollisionEnabled() && (!sceneComponent->IsPhysicsStateCreated() || sceneComponent->IsAsyncCollisionCookingPending());

			if (!isRenderStatePending && !isPhysicsStatePending)
			{
				sceneComponent->ClearMeshDataBuffers();
				Iter.RemoveCurrent();
			}
		}
		
		
	}


	//create runtime meshes from queue populated by worker mesh gen threads
	if (!mTesselatedMeshQueue.IsEmpty())
	{
		using namespace builder;
		AActor* owner = GetOwner();
		USceneComponent* sceneComponent = owner->GetRootComponent();

		builder::meshinfo::MeshInfo OutMeshInfo;
		while (mTesselatedMeshQueue.Dequeue(OutMeshInfo))
		{
			switch (OutMeshInfo.meshType)
			{
			case EMeshType::E_MESHTYPE_TERRAIN: 
			{	
				CreateTerrainRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;	
			}
			case EMeshType::E_MESHTYPE_LEAF: 
			{	
				CreateLeavesRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;	
			}
			case EMeshType::E_MESHTYPE_WATER: 
			{	
				CreateWaterRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_LAVA: 
			{	
				CreateLavaRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_TERRAIN_SHADOW: 
			{	
				CreateShadowTerrainRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_LEAF_SHADOW: 
			{	
				CreateShadowLeavesRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_WATER_SHADOW: 
			{	
				CreateShadowWaterRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_LAVA_SHADOW: 
			{	
				CreateShadowLavaRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_INVISIBLE_WALL: 
			{	
				CreateInvisibleWallRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_INVISIBLE_KILLZONE: 
			{	
				CreateInvisibleKillZoneRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			case EMeshType::E_MESHTYPE_TERRAIN_FILL: 
			{	
				CreateTerrainFillRuntimeMesh(OutMeshInfo, owner, sceneComponent);
				break;
			}
			default:break;
			}

		}
	}

}

void ULovikaGeneratorBuilder::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessGeneratedWorldMeshes();
	
	if (const bool bHaveMeshGenTasksCompleted = HaveMeshGenTasksCompleted())
	{
		FPlatformMisc::MemoryBarrier();
		SetComponentTickEnabled(false);
		SetCreatedWorldMeshes();

		const auto game = actorquery::getFirstActor<AGameBP>(GetWorld());
		if (game)
		{			
			game->NotifyWorldMeshesCreated();
		}

		DumpExcessMeshData();

		//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("## ULovikaGeneratorBuilder mTotalTris %d mTotalShadowTris %d mSimpleCollisionVolumes %d\n"), mTotalTris, mTotalShadowTris, mSimpleCollisionVolumes);

#if SHOW_LOAD_TIMES
		GEN_TEST_TIME(bHaveMeshGenTasksCompleted);
		GEN_TEST_TIME_PRINT(bHaveMeshGenTasksCompleted);
		GEN_TEST_TIME_PRINT(bmMeshGenJobs);
		GEN_TEST_TIME_PRINT(bmTesselatedMeshQueue);
		GEN_TEST_TIME_PRINT(bmUnparsedRuntimeMeshes);
#endif

	}

}



void rewriteMeshIntoFrontFacingAndCollisionData(builder::meshinfo::MeshSectionData& meshData, bool keepCollision) {
	meshData.collisionVertices = std::move(meshData.vertices);
	meshData.collisionIndices = std::move(meshData.indices);

	TArray<bool> isVertexWithIndexIncluded = Util::createZeroedTArrayOfSize<bool>(meshData.collisionIndices.Num());

	const auto vertexIndexPredicate = [&](int i) {
		const auto& normal = meshData.normals[i];
		return normal.X < -0.5f || normal.Y < -0.5f || normal.Z > 0.5f;
	};

	for (int i = 0; i < meshData.collisionIndices.Num(); i += 3) {
		if (vertexIndexPredicate(meshData.collisionIndices[i])) {
			meshData.indices.Append(&meshData.collisionIndices[i], 3);

			for (int j = 0; j < 3; ++j) {
				isVertexWithIndexIncluded[meshData.collisionIndices[i + j]] = true;
			}
		}
	}
	{
		// Update indices where discarded vertex indexes are removed
		TArray<int> recalculatedVertexIndices = Util::createUninitializedTArrayOfSize<int>(meshData.collisionVertices.Num());
		for (int i = 0, j = 0; i < recalculatedVertexIndices.Num(); ++i) {
			recalculatedVertexIndices[i] = j;

			if (isVertexWithIndexIncluded[i]) {
				++j;
			}
		}
		for (auto& index : meshData.indices) {
			index = recalculatedVertexIndices[index];
		}
	}

	// This could be improved (performance-wise) by:
	//  - Doing inline swaps (but would have to change indices accordingly)
	//  - Update BlockTesselator to output multiple buffers. If we use facing + back-facing we could do without this pass completely

	const auto includedVertexIndices = [&] {
		auto indices = Util::createTArrayOfReservedSize<int>(algo::count(isVertexWithIndexIncluded, true));
		for (int i = 0; i < isVertexWithIndexIncluded.Num(); ++i) {
			if (isVertexWithIndexIncluded[i]) {
				indices.Add(i);
			}
		}
		return indices;
	}();

	const auto filterComponents = [&](auto& meshDataCollection, auto temp) {
		temp.Reserve(includedVertexIndices.Num());
		for (auto i : includedVertexIndices) {
			temp.Add(meshDataCollection[i]);
		}
		meshDataCollection = std::move(temp);
	};

	filterComponents(meshData.normals, TArray<FVector>{});
	filterComponents(meshData.colors, TArray<FColor>{});
	filterComponents(meshData.uv1, TArray<FVector2D>{});

	for (auto i : includedVertexIndices) {
		meshData.vertices.Add(meshData.collisionVertices[i]);
	}

	if (!keepCollision) {
		meshData.collisionVertices.Empty();
		meshData.collisionIndices.Empty();
	}
}


void ULovikaGeneratorBuilder::AddBasicCollisionBoxesToMeshComponent(builder::meshinfo::MeshInfo& MeshData, URuntimeMeshComponent* mesh)
{
	//add basic Axis Aligned collision boxes
	for (FBox& CollisionBox : MeshData.basicAxisAlignedCollisionBoxes)
	{
		if (CollisionBox.IsValid)
		{
			FVector Extent = CollisionBox.Max - CollisionBox.Min;
			FVector Center = CollisionBox.Min + (Extent * 0.5f);
			FRuntimeMeshCollisionBox NewBox(Extent.X, Extent.Y, Extent.Z);
			NewBox.Center = Center;
			mesh->AddCollisionBox(NewBox);
			++mSimpleCollisionVolumes;
		}
	}
	
	//add basic collision boxes
	for (auto& CollisionBox : MeshData.basicCollisionBoxes)
	{
		FRuntimeMeshCollisionBox NewBox(CollisionBox.X, CollisionBox.Y, CollisionBox.Z);
		NewBox.Center = CollisionBox.Center;
		NewBox.Rotation = CollisionBox.Rotation;
		mesh->AddCollisionBox(NewBox);
		++mSimpleCollisionVolumes;
	}
}

void ULovikaGeneratorBuilder::rewriteMeshIntoFrontFacingAndCollisionDataAndRender(builder::meshinfo::MeshSectionInfo& section, URuntimeMeshComponent& mesh, bool canHideBackFaces) {
	const bool enabled = [&] {
		const auto enabledOverride = CVarHardBackFaceCulling.GetValueOnAnyThread();
		if (enabledOverride <= 0) { return false; }
		if (enabledOverride == 1) { return canHideBackFaces; }
		if (enabledOverride >= 2) { return true; }
		return false;
	}();

	mTotalTris += (section.meshData->indices.Num() / 3);

	if (enabled) {
		rewriteMeshIntoFrontFacingAndCollisionData(*section.meshData, section.bGenerateComplexCollision);

		mesh.CreateMeshSection(section.sectionIndex, 0, section.meshData->vertices, section.meshData->indices, section.meshData->normals, section.meshData->uv1, section.meshData->colors, TArray<FRuntimeMeshTangent>(), false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
		if (section.bGenerateComplexCollision) {
			check(section.meshData->collisionVertices.Num() > 0);
			mesh.TransferMeshCollisionSection(section.sectionIndex, section.meshData->collisionVertices, section.meshData->collisionIndices);
		}
	}
	else {
		check(section.meshData->vertices.Num() > 0);
		mesh.CreateMeshSection(section.sectionIndex, 0,section.meshData->vertices, section.meshData->indices, section.meshData->normals, section.meshData->uv1, section.meshData->colors, TArray<FRuntimeMeshTangent>(), section.bGenerateComplexCollision, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
	}

}

void ULovikaGeneratorBuilder::CreateTerrainRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	if (SourceMesh.meshSectionInfo.size() == 0)
	{
		// if there are no sections then the mesh has no geometry
		return;
	}
	FName meshName(("Mesh_" + std::to_string(SourceMesh.meshNum) + "_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* mesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, true, false, SourceMesh.sharedData.blockGraphicsPack);
	
	mesh->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::IgnoreTerrain, ECollisionResponse::ECR_Ignore);
	mesh->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly, ECollisionResponse::ECR_Block);
	
	bool bDoComplexCollision = false;

	for (auto& section : SourceMesh.meshSectionInfo)
	{
		bDoComplexCollision |= section.bGenerateComplexCollision;

		//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Attempting to create section %d of mesh %s shadow %d\n"), section.sectionIndex, meshNameTChar.GetData(), (int)section.bCastShadow);
		rewriteMeshIntoFrontFacingAndCollisionDataAndRender(section, *mesh, SourceMesh.sharedData.canHideBackFaces);

		if (builder::IsValidMaterial(section.material))
		{
			mesh->SetMaterial(section.sectionIndex, section.material);
		}
		mesh->SetMeshSectionCastsShadow(section.sectionIndex, section.bCastShadow);
	}

	AddBasicCollisionBoxesToMeshComponent(SourceMesh, mesh);

	//restore async cooking to mesh if any of the sections require it to build complex collision geometry
	mesh->SetCollisionUseComplexAsSimple(bDoComplexCollision);
	mesh->SetCollisionUseAsyncCooking(bDoComplexCollision);

}

void ULovikaGeneratorBuilder::CreateLeavesRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];
	builder::meshinfo::MeshSectionData& sectionData = *section.meshData;

	FName meshName(("Mesh_" + std::to_string(SourceMesh.meshNum) + "_Leaves_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* leavesMesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, false, section.bGenerateComplexCollision, SourceMesh.sharedData.blockGraphicsPack);
	
	leavesMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	leavesMesh->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly, ECollisionResponse::ECR_Overlap);
	
	//leavesMesh->CreateMeshSection(0, 0, sectionData.vertices, sectionData.indices, sectionData.normals, sectionData.uv1 /*, section.meshData.uv2*/, sectionData.colors, TArray<FRuntimeMeshTangent>(), section.bGenerateCollision, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
	rewriteMeshIntoFrontFacingAndCollisionDataAndRender(section, *leavesMesh, SourceMesh.sharedData.canHideBackFaces);
	AddBasicCollisionBoxesToMeshComponent(SourceMesh, leavesMesh);
	leavesMesh->SetRenderCustomDepth(false);
	
	if (builder::IsValidMaterial(section.material))
	{
		leavesMesh->SetMaterial(0, section.material);
	}

	leavesMesh->SetMeshSectionCastsShadow(0, section.bCastShadow);
}

void ULovikaGeneratorBuilder::CreateWaterRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	FName meshName(("Mesh_" + std::to_string(SourceMesh.meshNum) + "_Water_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* waterRuntimeMesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, false, false, SourceMesh.sharedData.blockGraphicsPack);
	
	waterRuntimeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	waterRuntimeMesh->SetRenderCustomDepth(false);
	waterRuntimeMesh->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_8);
	waterRuntimeMesh->SetCustomDepthStencilValue(0x08);

	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];

	//waterRuntimeMesh->CreateMeshSection(0, 0, section.meshData->vertices, section.meshData->indices, section.meshData->normals, section.meshData->uv1, section.meshData->colors, TArray<FRuntimeMeshTangent>(), section.bGenerateCollision, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
	rewriteMeshIntoFrontFacingAndCollisionDataAndRender(section, *waterRuntimeMesh, SourceMesh.sharedData.canHideBackFaces);
	if (builder::IsValidMaterial(section.material))
	{
		waterRuntimeMesh->SetMaterial(0, section.material);
	}

	waterRuntimeMesh->SetMeshSectionCastsShadow(0, section.bCastShadow);

}

void ULovikaGeneratorBuilder::CreateLavaRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	FName meshName(("Mesh_" + std::to_string(SourceMesh.meshNum) + "_Lava_" + SourceMesh.position.toString()).c_str());
	FActorSpawnParameters Params;
	Params.Name = meshName;
	AStaticMeshActor* staticLavaMesh = owner->GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), owner->GetTransform(), Params);
	staticLavaMesh->Tags.Add("NoInstancing");
	
	// lava always has one section
	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];
	builder::meshinfo::MeshSectionData& sectionData = *section.meshData;
	
	FName meshComponentName(("Mesh_" + std::to_string(SourceMesh.meshNum) + "_Lava_Component_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* lavaMesh = CreateNewRuntimeMeshObject(staticLavaMesh, staticLavaMesh->GetRootComponent(), meshComponentName, true, section.bGenerateComplexCollision, SourceMesh.sharedData.blockGraphicsPack);
	
	lavaMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	rewriteMeshIntoFrontFacingAndCollisionDataAndRender(section, *lavaMesh, SourceMesh.sharedData.canHideBackFaces);
	
	AddBasicCollisionBoxesToMeshComponent(SourceMesh, lavaMesh);

	lavaMesh->SetRenderCustomDepth(false);
	lavaMesh->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_8);
	lavaMesh->SetCustomDepthStencilValue(0x08);
	
	if (builder::IsValidMaterial(section.material))
	{
		lavaMesh->SetMaterial(0, section.material);
	}
	
	lavaMesh->SetMeshSectionCastsShadow(0, section.bCastShadow);
	
	//Add Nav Mesh Modifier
	UNavModifierComponent* LavaModifier = NewObject<UNavModifierComponent>(staticLavaMesh);
	LavaModifier->ComponentTags.Add(FName("LavaModifier"));
	LavaModifier->SetAreaClass(UNavAreaLava::StaticClass());
	LavaModifier->bIncludeAgentHeight = true;
	LavaModifier->RegisterComponent();

}

void ULovikaGeneratorBuilder::CreateShadowTerrainRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	FName meshName(("FS_Shadow_Mesh_" + std::to_string(SourceMesh.meshNum) + "_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* mesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, false, false, SourceMesh.sharedData.blockGraphicsPack);
	
	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	mesh->bCastHiddenShadow = 1;
	mesh->bHiddenInGame = 1;
	
	for (auto& section : SourceMesh.meshSectionInfo)
	{
		mTotalShadowTris += (section.meshData->indices.Num() / 3);
		mesh->CreateMeshSection(section.sectionIndex, URuntimeMeshComponent::CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID, section.meshData->vertices, section.meshData->indices, section.meshData->normals, section.meshData->uv1, section.meshData->colors, TArray<FRuntimeMeshTangent>(), false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
		mesh->SetMeshSectionCastsShadow(section.sectionIndex, section.bCastShadow);
	}

}

void ULovikaGeneratorBuilder::CreateShadowLeavesRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	// leaf shadows always have 1 section
	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];
	builder::meshinfo::MeshSectionData& sectionData = *section.meshData;

	FName meshName(("FS_Shadow_Mesh_" + std::to_string(SourceMesh.meshNum) + "_Water_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* leavesMesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, false, false, SourceMesh.sharedData.blockGraphicsPack);

	leavesMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	leavesMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	mTotalShadowTris += (sectionData.indices.Num() / 3);
	leavesMesh->CreateMeshSection(0, URuntimeMeshComponent::CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID, sectionData.vertices, sectionData.indices, sectionData.normals, sectionData.uv1, sectionData.colors, TArray<FRuntimeMeshTangent>(), false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);

	leavesMesh->SetRenderCustomDepth(false);
	leavesMesh->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_8);
	leavesMesh->SetCustomDepthStencilValue(0x08);
	
	if (builder::IsValidMaterial(section.material))
	{
		leavesMesh->SetMaterial(0, section.material);
	}

	leavesMesh->SetMeshSectionCastsShadow(0, section.bCastShadow);
	leavesMesh->bCastHiddenShadow = 1;
	leavesMesh->bHiddenInGame = 1;

}

URuntimeMeshComponent* ULovikaGeneratorBuilder::CreateNewRuntimeMeshObject(AActor* owner, USceneComponent* sceneComponent, const FName& meshName, bool CanEverAffectNavigation, bool bDoComplexCollision, const BlockGraphicsPack* blockGraphicsPack)
{
	//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Attempting to create mesh %s\n"), *meshName.ToString());

	URuntimeMeshComponent* pOut = NewObject<URuntimeMeshComponent>(owner, meshName);
	
	mUnparsedRuntimeMeshes.Push(pOut);

	pOut->RegisterComponent();
	pOut->SetComponentTickEnabled(false);
	pOut->AttachToComponent(sceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	pOut->SetMobility(EComponentMobility::Static);
	pOut->SetRelativeScale3D(FVector(builder::meshinfo::scale, builder::meshinfo::scale, builder::meshinfo::scale));
	pOut->SetCollisionUseComplexAsSimple(bDoComplexCollision);
	pOut->SetCollisionUseAsyncCooking(bDoComplexCollision);
	pOut->SetCanEverAffectNavigation(CanEverAffectNavigation);
	
	if (blockGraphicsPack) {
		pOut->SetUserData(std::make_shared<const BlockGraphicsPack*>(blockGraphicsPack));
	}
	return pOut;
}

void ULovikaGeneratorBuilder::CreateShadowWaterRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{

	FName meshName(("FS_Shadow_Mesh_" + std::to_string(SourceMesh.meshNum) + "_Water_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* waterMesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, false, false, SourceMesh.sharedData.blockGraphicsPack);
	
	waterMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	waterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	waterMesh->SetRenderCustomDepth(false);
	waterMesh->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_8);
	waterMesh->SetCustomDepthStencilValue(0x08);

	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];

	mTotalShadowTris += (section.meshData->indices.Num() / 3);
	waterMesh->CreateMeshSection(0, URuntimeMeshComponent::CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID, section.meshData->vertices, section.meshData->indices, section.meshData->normals, section.meshData->uv1, section.meshData->colors, TArray<FRuntimeMeshTangent>(), false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
	if (builder::IsValidMaterial(section.material))
	{
		waterMesh->SetMaterial(0, section.material);
	}
	waterMesh->SetMeshSectionCastsShadow(0, section.bCastShadow);

	waterMesh->bCastHiddenShadow = 1;
	waterMesh->bHiddenInGame = 1;

}

void ULovikaGeneratorBuilder::CreateShadowLavaRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	// lava shadow meshes only have one section
	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];
	builder::meshinfo::MeshSectionData& sectionData = *section.meshData;
	// Create a lava parent actor
	AActor* lavaActor = owner;

	FName meshName(("FS_Shadow_Mesh_" + std::to_string(SourceMesh.meshNum) + "_Lava_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* lavaMesh = CreateNewRuntimeMeshObject(lavaActor, sceneComponent, meshName, false, false, SourceMesh.sharedData.blockGraphicsPack);
	
	lavaMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	lavaMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (section.bCastShadow)
	{
		mTotalShadowTris += (sectionData.indices.Num()/3);
		lavaMesh->CreateMeshSection(0, URuntimeMeshComponent::CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID, sectionData.vertices, sectionData.indices, sectionData.normals, sectionData.uv1, sectionData.colors, TArray<FRuntimeMeshTangent>(), false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
	}

	lavaMesh->SetRenderCustomDepth(false);
	lavaMesh->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_8);
	lavaMesh->SetCustomDepthStencilValue(0x08);

	if (builder::IsValidMaterial(section.material))
	{
		lavaMesh->SetMaterial(0, section.material);
	}

	lavaMesh->SetMeshSectionCastsShadow(0, section.bCastShadow);
	lavaMesh->bCastHiddenShadow = 1;
	lavaMesh->bHiddenInGame = 1;

}

void ULovikaGeneratorBuilder::CreateInvisibleWallRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{	
	bool bGenComplexColision = false;
	UE_LOG(LogTemp, Display, TEXT("Creating Invisible wall mesh"));

	FName meshName(("InvisibleWall_" + SourceMesh.sharedData.customName + "_" + std::to_string(SourceMesh.meshNum) + "_" + SourceMesh.position.toString()).c_str());
	FActorSpawnParameters Params;
	Params.Name = meshName;
	AStaticMeshActor* staticMesh = owner->GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), owner->GetTransform(), Params);
	staticMesh->Tags.Add("NoInstancing");


	FName meshComponentName(("InvisibleWall_" + SourceMesh.sharedData.customName + "_Component_" + std::to_string(SourceMesh.meshNum) + "_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* mesh = CreateNewRuntimeMeshObject(staticMesh, staticMesh->GetRootComponent(), meshComponentName, true, bGenComplexColision, SourceMesh.sharedData.blockGraphicsPack);
	
	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	mesh->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECollisionResponse::ECR_Block);

	mesh->SetVisibility(ShowInvisibleWalls >= 1);
	mesh->SetCastShadow(false);
	for (auto& sectionInfo : SourceMesh.meshSectionInfo)
	{
		auto& sectionData = sectionInfo.meshData;
		mesh->CreateMeshSection(
			sectionInfo.sectionIndex, 0,
			sectionData->vertices,
			sectionData->indices,
			sectionData->normals,
			sectionData->uv1,
			sectionData->colors,
			TArray<FRuntimeMeshTangent>(),
			bGenComplexColision,  // generateCollision
			EUpdateFrequency::Infrequent,
			ESectionUpdateFlags::None,
			false,
			builder::UseFullPrecisionUVs
		);

		if (builder::IsValidMaterial(sectionInfo.material))
		{
			mesh->SetMaterial(0, sectionInfo.material);
			mesh->SetMaterial(sectionInfo.sectionIndex, sectionInfo.material);
		}
	}

	AddBasicCollisionBoxesToMeshComponent(SourceMesh, mesh);

	//Add Nav Mesh Modifier
	UNavModifierComponent* NavModifierComponent = NewObject<UNavModifierComponent>(staticMesh);
	NavModifierComponent->ComponentTags.Add(FName("NavModifier"));
	NavModifierComponent->bIncludeAgentHeight = true;
	NavModifierComponent->RegisterComponent();

}

void ULovikaGeneratorBuilder::CreateInvisibleKillZoneRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	UE_LOG(LogTemp, Display, TEXT("Creating invisible killzone mesh"));
	FName meshName(("KillZone_" + SourceMesh.sharedData.customName + "_" + std::to_string(SourceMesh.meshNum) + "_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* mesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, true, false, SourceMesh.sharedData.blockGraphicsPack);
	
	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	mesh->SetCollisionObjectType((ECollisionChannel)ECustomTraceChannels::KillzoneBlocker);
	mesh->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECollisionResponse::ECR_Block);
	
	mesh->SetVisibility(ShowKillZones >= 1);
	mesh->SetCastShadow(false);

	for (auto& sectionInfo : SourceMesh.meshSectionInfo)
	{
		auto& sectionData = sectionInfo.meshData;
		mesh->CreateMeshSection(
			sectionInfo.sectionIndex, 0,
			sectionData->vertices,
			sectionData->indices,
			sectionData->normals,
			sectionData->uv1,
			sectionData->colors,
			TArray<FRuntimeMeshTangent>(),
			false,  // generateCollision
			EUpdateFrequency::Infrequent,
			ESectionUpdateFlags::None,
			false,
			builder::UseFullPrecisionUVs
		);

		if (builder::IsValidMaterial(sectionInfo.material))
		{
			mesh->SetMaterial(0, sectionInfo.material);
			mesh->SetMaterial(sectionInfo.sectionIndex, sectionInfo.material);
		}
	}

	AddBasicCollisionBoxesToMeshComponent(SourceMesh, mesh);
}

void ULovikaGeneratorBuilder::CreateTerrainFillRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent)
{
	if (SourceMesh.meshSectionInfo.size() == 0)
	{
		// if there are no sections then the mesh has no geometry
		return;
	}

	builder::meshinfo::MeshSectionInfo& section = SourceMesh.meshSectionInfo[0];

	FName meshName(("Mesh_" + std::to_string(SourceMesh.meshNum) + "_FILL_" + SourceMesh.position.toString()).c_str());
	URuntimeMeshComponent* mesh = CreateNewRuntimeMeshObject(owner, sceneComponent, meshName, false, false, SourceMesh.sharedData.blockGraphicsPack);

	mesh->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::IgnoreTerrain, ECollisionResponse::ECR_Ignore);
	mesh->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly, ECollisionResponse::ECR_Block);

	mesh->SetRenderCustomDepth(false);
	//mesh->CreateMeshSection(0, 0, section.meshData->vertices, section.meshData->indices, section.meshData->normals, section.meshData->uv1, section.meshData->colors, TArray<FRuntimeMeshTangent>(), section.bGenerateCollision, EUpdateFrequency::Infrequent, ESectionUpdateFlags::None, false, builder::UseFullPrecisionUVs);
	rewriteMeshIntoFrontFacingAndCollisionDataAndRender(section, *mesh, SourceMesh.sharedData.canHideBackFaces);
	AddBasicCollisionBoxesToMeshComponent(SourceMesh, mesh);

	if (builder::IsValidMaterial(section.material))
	{
		mesh->SetMaterial(0, section.material);
	}
	mesh->SetMeshSectionCastsShadow(0, false);
}

bool ULovikaGeneratorBuilder::HasGeneratedWorldMeshes() const
{
	return mMeshesGenerated;
}

void ULovikaGeneratorBuilder::AbortMeshGeneration()
{
	AbortAndWait();
}

void ULovikaGeneratorBuilder::AbortAndWait()
{
	if(!HaveMeshGenTasksCompleted())
	{
		bool wait = false;
		//Cancel all jobs that have not yet started
		for(const auto& job : mMeshGenJobs)
		{
			//Cancel will return false if the task is currently running and cannot be removed from the queue.
			wait |= !job->Cancel();
		}

		//If at least one job is running, we wait set the abort flag and wait for completion.
		if(wait)
		{
			_abortFlag = true;
			for(const auto& job : mMeshGenJobs)
			{
				job->EnsureCompletion();
			}	
		}
		FPlatformMisc::MemoryBarrier();
		mMeshGenJobs.clear();
		mTesselatedMeshQueue.Empty();
		mUnparsedRuntimeMeshes.Empty();
	}
	_abortFlag = false;
}

void ULovikaGeneratorBuilder::_clearMeshes() {
	AActor* owner = GetOwner();
	
	TArray<AActor*> childActors;
	owner->GetAttachedActors(childActors);
	for (AActor* actor : childActors) {
		actor->Destroy();
	}
	TArray<UActorComponent*> meshComponents = owner->GetComponentsByClass(URuntimeMeshComponent::StaticClass());
	for (UActorComponent* meshComponent : meshComponents) {
		meshComponent->DestroyComponent();
	}
	mTesselatedMeshQueue.Empty();
	mUnparsedRuntimeMeshes.Empty();
	mMeshesGenerated = false;
}
