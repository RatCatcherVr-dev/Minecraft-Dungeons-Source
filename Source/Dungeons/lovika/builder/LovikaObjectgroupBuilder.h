#pragma once

#include "LovikaBaseBuilder.h"
#include "CommonTypes.h"
#include "lovika/io/ObjectGroupFile.h"
#include "LovikaObjectgroupBuilder.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;
class BlockSource;

UCLASS()
class DUNGEONS_API ULovikaObjectgroupBuilder : public ULovikaBaseBuilder {
	GENERATED_BODY()
public:
	void build(bool useAmbientOcclusion, FString objectGroupName, bool rebuildMeshes, const TOptional<TArray<FString>>& allowedTileIds = TOptional<TArray<FString>>());	
	void build(bool useAmbientOcclusion, Unique<io::ObjectGroup>, bool rebuildMeshes, const TOptional<TArray<FString>>& allowedTileIds = TOptional<TArray<FString>>());

	BlockSource* getBlockSource() { return region; }
	io::ObjectGroup* getObjectGroupPtr();

private:
	static bool isTileAllowed(const std::string& tileId, const TOptional<TArray<FString>>& allowedTileIds);

	std::unique_ptr<io::ObjectGroup> objectGroup;
	BlockSource* region;
};
