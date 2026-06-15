#include "Dungeons.h"

#include "DecorationLevelActor.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "editor/debug/RegionRenderer.h"
#include "game/Conversion.h"
#include "game/GameTypes.h"
#include "game/util/ActorQuery.h"
#include "lovika/builder/LovikaObjectgroupBuilder.h"
#include "lovika/Region.h"
#include "lovika/ObjectGroupQuery.h"
#include "lovika/io/ObjectGroupFile.h"
#include "lovika/LovikaObjectgroupLevelActor.h"
#include "world/level/BlockSource.h"
#include <Engine/LevelStreaming.h>
#include <Object.h>

ADecorationLevelActor::ADecorationLevelActor() {
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));

	static ConstructorHelpers::FClassFinder<AActor> fireBPClassFinder(*(game::PrefabPath("Decor/Prefabs/Blueprints/BP_Fire")));
	fireBPClass = fireBPClassFinder.Class;

	mBuilder = CreateDefaultSubobject<ULovikaObjectgroupBuilder>(TEXT("ObjectgroupBuilder"));
}

namespace {
FString getObjectgroupFilename(const FString& name) {
	auto fullName = FPaths::ProjectDir() + "Content/data/lovika/objectgroups/" + name;

	if (!FPaths::FileExists(fullName) && FPaths::DirectoryExists(fullName)) {
		return name + TEXT("/objectgroup.json");
	}
	return name;
}
}

static FString RightOfLast(const FString& s, char delimiter) {
	int index;
	return s.FindLastChar(delimiter, index) ? s.RightChop(index + 1) : s;
}

void ADecorationLevelActor::rebuildLevel(bool rebuildMeshes) {
	if (!doRefresh) {
		return;
	}
	mBuilder->init({ ResourcePack(resourcePack) });
	DebugAtlasTexture = mBuilder->getMostRecentResourcePackTextureAtlas().atlasTexture;
	DebugAtlasTextureEmissive = mBuilder->getMostRecentResourcePackTextureAtlas().atlasTextureEmissive;
	mBuilder->build(true, getObjectgroupFilename(objectgroup), rebuildMeshes, commaSeparatedTileIdsStringToOptionalArray(tileIds));
	RegisterAllComponents();
	doRefresh = false;

	if (auto group = mBuilder->getObjectGroupPtr()) {
		mObjects.clear();
		std::transform(group->objects.begin(), group->objects.end(), std::back_inserter(mObjects), [](const auto& o){ return &o; });

		auto levels = GetWorld()->GetStreamingLevels();

		for (ULevelStreaming* level : levels) {
			auto packageName = level->GetWorldAssetPackageName();
			auto name = RightOfLast(packageName, '/');
			name.ToLowerInline();

			auto it = std::find_if(group->objects.begin(), group->objects.end(), [&name](const io::Object& o) {
				return FString(Util::toLower(o.id).c_str()) == name;
			});
			if (it == group->objects.end()) {
				continue;
			}
			level->LevelTransform.SetLocation(conversion::blockToUe(it->bounds.minInclusive));
		}
	}
}

void ADecorationLevelActor::OnConstruction(const FTransform& transform) {
	Super::OnConstruction(transform);

	UE_LOG(LogTemp, Warning, TEXT("Running DecorationLevel::OnConstruction"));
	rebuildLevel(true);

	mRegionRenderer = std::make_unique<RegionRenderer>(*GetRootComponent());
}

void ADecorationLevelActor::PreInitializeComponents() {
	UE_LOG(LogTemp, Warning, TEXT("Running DecorationLevel::PreInitializeComponents"));
 	rebuildLevel(false);
}

void ADecorationLevelActor::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Running DecorationLevel::BeginPlay"));
}

void ADecorationLevelActor::Tick( float DeltaTime ) {
	Super::Tick(DeltaTime);
}

const io::Object* ADecorationLevelActor::getObjectFor(BlockPos pos) const {
	return objectgroupquery::getObject(*mBuilder->getObjectGroupPtr(), pos);
}

const std::vector<const io::Object*>& ADecorationLevelActor::getAllObjects() const {
	return mObjects;
}

void ADecorationLevelActor::ToggleRegions() {
	ShowRegions = !ShowRegions;

	if (!mRegionRenderer) {
		return;
	}
	mRegionRenderer->clear();

	if (ShowRegions) {
		for (const auto obj : mObjects) {
			const auto basePos = obj->bounds.minInclusive;
			for (const auto& region : obj->regions) {
				mRegionRenderer->add(lovika::Region(region.name, region.type, region.bounds + basePos, ""));
			}
			for (const auto& region : obj->doors) {
				mRegionRenderer->add(lovika::Region(region.name, regiontype::Door, region.bounds + basePos, ""));
			}
		}
	}
}
