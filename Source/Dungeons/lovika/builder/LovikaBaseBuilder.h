#pragma once

#include "GameFramework/Actor.h"
#include "world/level/LevelConstants.h"
#include "LovikaBasebuilder.generated.h"


class UMaterialInterface;
class TextureAtlas;
class ResourcePack;
namespace game {
	class Game;
}

UCLASS()
class DUNGEONS_API ULovikaBaseBuilder : public UActorComponent {
	GENERATED_BODY()
	
public:
	ULovikaBaseBuilder();

	virtual void init(const std::vector<ResourcePack>&, game::Game* game = nullptr);

	void InitialiseMaterialInstances();

	const TextureAtlas& getMostRecentResourcePackTextureAtlas();
	
public:

	static const int RENDERCHUNK_SIDE_SCALED = RENDERCHUNK_SIDE * 100;

	UPROPERTY()
	UMaterialInterface* materialOpaque = nullptr;
	UPROPERTY()
	UMaterialInterface* materialAlphaTest = nullptr;
	UPROPERTY()
	UMaterialInterface* materialAlphaTestFoilage = nullptr;
	UPROPERTY()
	UMaterialInterface* materialLeaves = nullptr;
	UPROPERTY()
	UMaterialInterface* materialLeavesOpaque = nullptr;
	UPROPERTY()
	UMaterialInterface* materialReflective = nullptr;
	UPROPERTY()
	UMaterialInterface* materialEmissive = nullptr;
	UPROPERTY()
	UMaterialInterface* materialWater = nullptr;

	UPROPERTY()
	UMaterialInterface* materialFSShadowOpaque = nullptr;
	UPROPERTY()
	UMaterialInterface* materialFSShadowLeaves = nullptr;

	UPROPERTY(transient)	
	UMaterialInstanceDynamic* VoxelMatFSShadowMaterialInstanceOpaque = nullptr;	
	UPROPERTY(transient)	
	UMaterialInstanceDynamic* VoxelMatFSShadowMaterialInstanceLeaves = nullptr;

	UPROPERTY(transient)
	UMaterialInterface* materialVoid = nullptr;

	UPROPERTY(transient)
	UMaterialInterface* materialSolid = nullptr;	//D11:MR Solid color for hidden objects

	void OnComponentCreated() override;

	void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	void OnUnregister() override;
	virtual void AbortMeshGeneration();

	UMaterialInterface* GetOverrideMaterial(const FString& key) const;
protected:
	AActor* parentActor = nullptr;

	UMaterialInstanceDynamic* GenerateLeafMaterial();
	void OnLeafAlphaSettingChanged(IConsoleVariable*);
private:
	void CleanUpBlockGraphicsManager();
	UWorld* blockManagerKey = nullptr;

	UPROPERTY()
	TMap<FString, UMaterialInterface*> overrideMaterials;
};

