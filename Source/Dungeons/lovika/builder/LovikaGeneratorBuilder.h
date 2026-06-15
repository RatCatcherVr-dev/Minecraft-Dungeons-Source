#pragma once

#include "LovikaBaseBuilder.h"
#include "lovika/world/level/levelgen/LevelDef.h"
#include "lovika/world/level/levelgen/TileSet.h"
#include "game/level/GameTiles.h"

#include "game/GameBP.h"

#include "RuntimeMeshComponent.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"
#include "LovikaGeneratorBuilder.generated.h"

namespace io {
	struct ObjectGroup;
}
class UMaterialInterface;
class UMaterialInstanceDynamic;
class ChunkBlockSource;


namespace builder {
// D11.DH
// store all of the data needed for the post load callback which will execute on the game thread, this isn't ideal but 
// this is necessary because the meshing has to be done on the game thread because runtime mesh component doesn't have async capabilities
namespace meshinfo
{
	constexpr int scale = 100;

	struct MeshSectionData
	{
		// D11.DH
		// all mesh geometry data for the mesh section
		// this may be expensive to copy so try to avoid copies where possible as TArray does a deep copy of the memory in its copy constructor 

		// These are now camera facing vertices, indices, normals, ...
		TArray<FVector> vertices;
		TArray<int32> indices;
		TArray<FVector> normals;
		TArray<FColor> colors;
		TArray<FVector2D> uv1;

		TArray<FVector> collisionVertices;
		TArray<int32> collisionIndices;
		
	};
	struct MeshSectionInfo
	{
		bool bGenerateComplexCollision{ false };
		bool bUseShadowLod{ false };
		bool bCastShadow{ false };
		int sectionIndex{ 0 };

		UMaterialInterface* material{ nullptr };

		// Shared pointer to avoid copies
		Unique<MeshSectionData> meshData;
	};

	enum class EMeshType : uint8
	{
		E_MESHTYPE_TERRAIN = 0
		, E_MESHTYPE_LEAF
		, E_MESHTYPE_WATER
		, E_MESHTYPE_LAVA
		, E_MESHTYPE_TERRAIN_SHADOW
		, E_MESHTYPE_LEAF_SHADOW
		, E_MESHTYPE_WATER_SHADOW
		, E_MESHTYPE_LAVA_SHADOW
		, E_MESHTYPE_INVISIBLE_WALL
		, E_MESHTYPE_INVISIBLE_KILLZONE
		, E_MESHTYPE_TERRAIN_FILL
		, E_MESHTYPE_MAX
	};

	struct MeshInfo
	{
		std::vector<MeshSectionInfo> meshSectionInfo;

		TArray< FBox > basicAxisAlignedCollisionBoxes;
		TArray< FKBoxElem > basicCollisionBoxes;

		BlockPos position;

		int meshNum = 0;
		EMeshType meshType = EMeshType::E_MESHTYPE_TERRAIN;

		struct SharedData
		{
			bool canHideBackFaces = false;
			const BlockGraphicsPack* blockGraphicsPack = nullptr;
			std::string customName;
			
		}
		sharedData;
	};
	
}
}


class ULovikaGeneratorBuilder;

class FLovikaTesselateTask : public FNonAbandonableTask {
	ULovikaGeneratorBuilder* baseBuilder = nullptr;
	AGameBP* game = nullptr;
	AActor* owner = nullptr;
	ChunkBlockSource* region = nullptr;
	const game::Tiles& tiles;
	bool useAmbientOcclusion;
	bool doSimpleShadows;
	const bool& abort;
	const BlockGraphicsPack& blockGraphicsPack;
	std::vector<game::TilePtr> subDungeonTiles;
	BlockCuboid subBounds;
	bool canHideBackFaces;

	void DoPrimaryWork();
	void DoShadowWork();

public:
	FLovikaTesselateTask(
		ULovikaGeneratorBuilder*,
		AGameBP*, 
		AActor* owner, 
		ChunkBlockSource*, 
		const game::Tiles& , 
		bool useAo, 
		bool doShadow, 
		const bool& abortFlag, 
		const BlockGraphicsPack&, 
		std::vector<game::TilePtr>&, 
		const BlockCuboid&,
		bool canHideBackFaces
	);

	void DoWork();

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FLovikaGeneratorBuilderTask, STATGROUP_ThreadPoolAsyncTasks); }

};

UCLASS()
class DUNGEONS_API ULovikaGeneratorBuilder : public ULovikaBaseBuilder {
	GENERATED_BODY()
public:
	ULovikaGeneratorBuilder();

	void build(bool useAmbientOcclusion, ChunkBlockSource*, const game::Tiles&, bool canHideBackFaces);

	const levelgen::LevelDef& getLevelDef() { return mLevelDef; }

	bool HaveMeshGenTasksCompleted() const;
	bool HasGeneratedWorldMeshes() const;
	
	void SetCreatedWorldMeshes();
	void DumpExcessMeshData();

	void BeginDestroy() override;

	void AddTesselatedMeshInfo(builder::meshinfo::MeshInfo& sourceMeshInfo);
	void ProcessGeneratedWorldMeshes();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void AbortMeshGeneration() override;
private:
	void AbortAndWait();
	bool _abortFlag;
		
	//Runtime Mesh Type Creation Functions
	URuntimeMeshComponent* CreateNewRuntimeMeshObject(AActor* owner, USceneComponent*, const FName& meshName, bool CanEverAffectNavigation, bool bDoComplexCollision, const BlockGraphicsPack*);

	void AddBasicCollisionBoxesToMeshComponent(builder::meshinfo::MeshInfo& MeshData, URuntimeMeshComponent* mesh);
	void rewriteMeshIntoFrontFacingAndCollisionDataAndRender(builder::meshinfo::MeshSectionInfo& section, URuntimeMeshComponent& mesh, bool canHideBackFaces);
	void CreateTerrainRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateLeavesRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateWaterRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateLavaRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateShadowTerrainRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateShadowLeavesRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateShadowWaterRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateShadowLavaRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateInvisibleWallRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateInvisibleKillZoneRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	void CreateTerrainFillRuntimeMesh(builder::meshinfo::MeshInfo& SourceMesh, AActor* owner, USceneComponent* sceneComponent);
	
	std::vector<Unique<FAsyncTask<FLovikaTesselateTask>>> mMeshGenJobs;


	void _clearMeshes();

	uint32 mTotalTris;
	uint32 mTotalShadowTris;
	uint32 mSimpleCollisionVolumes;
	TileSet mTileSet;
	levelgen::LevelDef mLevelDef;
	bool mMeshesGenerated = false;

	TQueue< builder::meshinfo::MeshInfo, EQueueMode::Mpsc > mTesselatedMeshQueue;
	TArray < TWeakObjectPtr< URuntimeMeshComponent > > mUnparsedRuntimeMeshes;
	
};
