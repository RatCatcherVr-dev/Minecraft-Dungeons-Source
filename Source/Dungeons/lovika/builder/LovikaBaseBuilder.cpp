#include "Dungeons.h"

#include "LovikaBaseBuilder.h"

#include "world/level/block/Block.h"
#include "world/level/material/Material.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "world/level/biome/Biome.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RuntimeMeshComponent.h"
#include "game/mission/MissionDef.h"
#include "client/resource/Resource.h"
#include "game/Game.h"


TAutoConsoleVariable<int32> CVarAlphaTestFoilage(
	TEXT("Dungeons.Perf.AlphaTestLeaves"),
	1,
	TEXT("Enable/Disable alpha test on leaves")
	TEXT("<= 0: off.\n")
	TEXT(">= 1: on.\n"),
	ECVF_Scalability);
// Empty callback to use for the alpha test foliage cvar, does nothing but allows setting the delegate to not point to an old object
static void EmptyCVarCallback(IConsoleVariable* var) {}

ULovikaBaseBuilder::ULovikaBaseBuilder() {
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(
		TEXT("Material'/Game/Content_Parents/Materials/VoxelMats/VoxelMat.VoxelMat'"));
	if (Material.Object != NULL) {
		materialOpaque = (UMaterial*)Material.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAlpha(
		TEXT("Material'/Game/Content_Parents/Materials/VoxelMats/VoxelMatAlpha.VoxelMatAlpha'"));
	if (MaterialAlpha.Object != NULL) {
		materialAlphaTest = (UMaterial*)MaterialAlpha.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAlphaFoilage(
		TEXT("Material'/Game/Content_Parents/Materials/VoxelMats/VoxelMatAlphaWithWind.VoxelMatAlphaWithWind'"));
	if (MaterialAlphaFoilage.Object != NULL) {
		materialAlphaTestFoilage = (UMaterial*)MaterialAlphaFoilage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialLeaves(
		TEXT("Material'/Game/Materials/VoxelMatLeaves.VoxelMatLeaves'"));
	if (MaterialLeaves.Object != NULL) {
		materialLeaves = (UMaterial*)MaterialLeaves.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialLeavesOpaque(
		TEXT("Material'/Game/Materials/VoxelMatLeavesOpaque.VoxelMatLeavesOpaque'"));
	if (MaterialLeaves.Object != NULL) {
		materialLeavesOpaque = (UMaterial*)MaterialLeavesOpaque.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialEmissive(
		TEXT("Material'/Game/Materials/VoxelMatEmissive.VoxelMatEmissive'"));
	if (MaterialEmissive.Object != NULL) {
		materialEmissive = (UMaterial*)MaterialEmissive.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialWater(
		TEXT("Material'/Game/Materials/VoxelMatWater.VoxelMatWater'"));
	if (MaterialWater.Object != NULL) {
		materialWater = (UMaterial*)MaterialWater.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFSShadow(
		TEXT("Material'/Game/Materials/VoxelMatFSShadow.VoxelMatFSShadow'"));
	if (MaterialFSShadow.Object != NULL) {
		materialFSShadowOpaque = (UMaterial*)MaterialFSShadow.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFSShadowLeaves(
		TEXT("Material'/Game/Materials/VoxelMatLeavesFSShadow.VoxelMatLeavesFSShadow'"));
	if (MaterialFSShadowLeaves.Object != NULL) {
		materialFSShadowLeaves = (UMaterial*)MaterialFSShadowLeaves.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialVoid(TEXT("Material'/Game/Materials/VoxelMatVoid.VoxelMatVoid'"));
	if (MaterialVoid.Object != NULL) {
		materialVoid = (UMaterialInterface*)MaterialVoid.Object;
	}

	//D11:MR Solid color for hidden objects
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialSolid(TEXT("Material'/Game/Materials/M_Color.M_Color'"));
	if (MaterialSolid.Object != NULL) {
		materialSolid = (UMaterialInterface*)MaterialSolid.Object;
	}

	//Override Materials
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialMuddyWater(
		TEXT("Material'/Game/Materials/VoxelMatMuddyWater.VoxelMatMuddyWater'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialMuddyWaterSwitch(
		TEXT("Material'/Game/Materials/VoxelMatMuddyWater_Switch.VoxelMatMuddyWater_Switch'"));
	if (MaterialMuddyWater.Object != NULL) {
		overrideMaterials.Add("MuddyWater", PLATFORM_SWITCH ? (UMaterialInterface*)MaterialMuddyWaterSwitch.Object : (UMaterialInterface*)MaterialMuddyWater.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialSwampWater(
		TEXT("Material'/Game/Materials/VoxelMatSwampWater.VoxelMatSwampWater'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialSwampWaterSwitch(
		TEXT("Material'/Game/Materials/VoxelMatSwampWater_Switch.VoxelMatSwampWater_Switch'"));
	if (MaterialSwampWater.Object != NULL) {
		overrideMaterials.Add("SwampWater", PLATFORM_SWITCH ? (UMaterialInterface*)MaterialSwampWaterSwitch.Object : (UMaterialInterface*)MaterialSwampWater.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialLiquidVoid(
		TEXT("Material'/Game/Materials/VoxelMatLiquidVoid.VoxelMatLiquidVoid'"));
	if (MaterialLiquidVoid.Object != NULL) {
		overrideMaterials.Add("LiquidVoid", (UMaterialInterface*)MaterialLiquidVoid.Object);
	}
}

void ULovikaBaseBuilder::init(const std::vector<ResourcePack>& resources, game::Game* game) {
	if (blockManagerKey && blockManagerKey != GetWorld()) {
		BlockGraphicsHelper::RemoveInstance(blockManagerKey); 
	}
	blockManagerKey = GetWorld();
	BlockGraphicsHelper::CreateInstance(blockManagerKey);
	Material::teardownMaterials();
	Block::teardownBlocks();
	Material::initMaterials();
	Block::initBlocks();
	Biome::initBiomes();
	for (const auto& resource : resources) {
		auto& pack = BlockGraphicsHelper::getInstance(blockManagerKey).add(resource);

		pack.CreateMaterials(
			GetOwner(),
			materialOpaque,
			materialAlphaTest,
			materialAlphaTestFoilage,
			materialLeaves);
	}

	if (game)
	{
		if (UMaterialInterface* lavaOverride = GetOverrideMaterial(game->missionDef().getLavaOverrideMaterial())) materialEmissive = lavaOverride;
		if (UMaterialInterface* waterOverride = GetOverrideMaterial(game->missionDef().getWaterOverrideMaterial())) materialWater = waterOverride;
	}

	VoxelMatFSShadowMaterialInstanceOpaque = UMaterialInstanceDynamic::Create(materialFSShadowOpaque, GetOwner());	
	VoxelMatFSShadowMaterialInstanceLeaves = UMaterialInstanceDynamic::Create(materialFSShadowLeaves, GetOwner());
}

void ULovikaBaseBuilder::OnComponentCreated() {
	Super::OnComponentCreated();
	CVarAlphaTestFoilage.AsVariable()->SetOnChangedCallback(
		FConsoleVariableDelegate::CreateUObject(
			this, &ULovikaBaseBuilder::OnLeafAlphaSettingChanged));
}

void ULovikaBaseBuilder::OnComponentDestroyed(bool bDestroyingHierarchy) {
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	CVarAlphaTestFoilage.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&EmptyCVarCallback));
	CleanUpBlockGraphicsManager();
}

void ULovikaBaseBuilder::OnUnregister() {
	Super::OnUnregister();
	CleanUpBlockGraphicsManager();
}

void ULovikaBaseBuilder::AbortMeshGeneration()
{
}

void ULovikaBaseBuilder::CleanUpBlockGraphicsManager() {
	if (blockManagerKey) {
		AbortMeshGeneration();

		BlockGraphicsHelper::RemoveInstance(blockManagerKey);
		blockManagerKey = nullptr;
	}
}


void ULovikaBaseBuilder::OnLeafAlphaSettingChanged(IConsoleVariable*) {
	if (!blockManagerKey) {
		return;
	}
	const auto blockGraphicsPackToMeshesMap = [&] {
		TMap<BlockGraphicsPack*, TArray<URuntimeMeshComponent*>> out;

		for (auto& component : GetOwner()->GetComponentsByClass(URuntimeMeshComponent::StaticClass())) {
			URuntimeMeshComponent* mesh = Cast<URuntimeMeshComponent>(component);
			if (UMaterialInterface* material = mesh->GetMaterial(0)) {
				UMaterial* parentMaterial = material->GetMaterial();

				if (parentMaterial == materialLeaves || parentMaterial == materialLeavesOpaque) {
					if (auto* resourcePack = mesh->GetUserData<BlockGraphicsPack*>()) {
						out.FindOrAdd(*resourcePack).Add(mesh);
					}
				}
			}
		}
		return out;
	}();

	const auto baseLeafMaterial = CVarAlphaTestFoilage.GetValueOnAnyThread() ? materialLeaves : materialLeavesOpaque;
	const auto owner = GetOwner();

	for (auto& blockGraphicsPackMeshes : blockGraphicsPackToMeshesMap) {
		auto* material = blockGraphicsPackMeshes.Key->GenerateLeafMaterial(baseLeafMaterial, owner);

		for (auto* mesh : blockGraphicsPackMeshes.Value) {
			mesh->SetMaterial(0, material);
		}
	}
}


const TextureAtlas& ULovikaBaseBuilder::getMostRecentResourcePackTextureAtlas() {
	return *BlockGraphicsHelper::getInstance(blockManagerKey).getLatest().terrainTextureAtlas;
}

UMaterialInterface* ULovikaBaseBuilder::GetOverrideMaterial(const FString& key) const
{
	if(auto material = overrideMaterials.Find(key))
	{
		return *material;
	}

	return nullptr;
}