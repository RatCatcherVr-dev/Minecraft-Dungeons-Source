#pragma once

#include "GameFramework/Actor.h"
#include "client/resource/Resource.h"
#include "editor/debug/RegionRenderer.h"
#include "world/level/BlockPos.h"
#include "lovika/builder/LovikaBaseBuilder.h"
#include "DecorationLevelActor.generated.h"

class ULovikaObjectgroupBuilder;

namespace io {
struct Object;
}

UCLASS()
class DUNGEONS_API ADecorationLevelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADecorationLevelActor();

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PreInitializeComponents() override;

	void ToggleRegions();

	const std::vector<const io::Object*>& getAllObjects() const;
	const io::Object* getObjectFor(BlockPos) const;

	FORCEINLINE bool IsShowingRegions() const { return ShowRegions; }

	TSubclassOf<AActor> fireBPClass;

	/** Dynamically created texture */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
	UTexture2D* DebugAtlasTexture;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
	UTexture2D* DebugAtlasTextureEmissive;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
	
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	bool doRefresh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FString objectgroup = "MooncoreCaverns";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	EResourcePack resourcePack = EResourcePack::Vanilla;

	/** Comma separated list of tile IDs to load. Leave empty to load all tiles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FString tileIds = "";
private:
	void rebuildLevel(bool rebuildMeshes);

	ULovikaObjectgroupBuilder* mBuilder = nullptr;
	
	std::vector<const io::Object*> mObjects;
	
	Unique<RegionRenderer> mRegionRenderer;

	TArray<class UTextRenderComponent*> mText;

	bool ShowRegions = false;
};
