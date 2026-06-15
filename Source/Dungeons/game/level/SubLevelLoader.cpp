#include "Dungeons.h"
#include "SubLevelLoader.h"
#include "SubLevelUtil.h"
#include "util/Algo.h"
#include <Engine/LevelStreamingDynamic.h>

namespace game { namespace sublevel {

const Loader::Config Loader::DefaultConfig{ 2, 0 };

Loader::Loader(UWorld& world, Config config /* = DefaultConfig */)
	: mWorld(world)
	, mConfig(config) {
}

Loader::~Loader() {
	mWorld.RemoveStreamingLevels(algo::copy_if_map_tarray(mItems,
		RETLAMBDA(it.status == Item::Started && it.level.Get()),
		RETLAMBDA(Cast<ULevelStreaming>(it.level.Get()))
	));
}

ULevelStreamingDynamic* Loader::schedule(const FString& path, const FString& uniqueName, const FTransform& transform) {
	if (auto level = sublevel::createUnloaded(mWorld, path, uniqueName)) {
		if (mItems.Num() < mConfig.immediatelyLoadFirstSublevelsCount) {
			level->bShouldBlockOnLoad = true;
		}
		level->LevelTransform = transform;
		mItems.Add({ path, TStrongObjectPtr<ULevelStreamingDynamic>(level), Item::NotLoaded });
		return level;
	}
	return nullptr;
}

TArray<ULevelStreamingDynamic*> Loader::allLevels() const {
	TArray<ULevelStreamingDynamic*> out;
	for (const auto& item : mItems) {
		out.Add(item.level.Get());
	}
	return out;
}

void Loader::update() {
	_updateLoadFinished();
	_updateStartLoadBlocking();
	_updateStartNonBlocking();
}

bool Loader::startLoad(ULevelStreamingDynamic& level) {
	auto* item = mItems.FindByPredicate([&level](auto& item) { return item.level.Get() == &level; });
	if (item && item->status == Item::NotLoaded) {
		_startLoad(*item);
		return true;
	}
	return false;
}

void Loader::registerLevelLoadedCallback(LevelLoadedCallback callback) {
	mLevelLoadedCallbacks.push_back(std::move(callback));
}

bool Loader::AreAllLevelsLoaded() const
{
	return algo::all_of(mItems, [](const auto& item) { return item.status == Item::Loaded; });
}

void Loader::_updateLoadFinished() {
	for (auto& item : mItems) {
		if (Item::Started == item.status && item.level->IsLevelLoaded() && item.level->IsLevelVisible()) {
			item.status = Item::Loaded;

			algo::for_each(mLevelLoadedCallbacks, [&item](const LevelLoadedCallback& callback) { callback(item.level.Get()); });
		}
	}
}

void Loader::_updateStartLoadBlocking() {
	for (auto& item : mItems) {
		if (Item::NotLoaded == item.status && item.level->bShouldBlockOnLoad) {
			_startLoad(item);
		}
	}
}

void Loader::_updateStartNonBlocking() {
	const int maxLoadCount = mConfig.maxParallelLoadCount >= 1 ? mConfig.maxParallelLoadCount : mItems.Num();
	int loadCount = maxLoadCount - algo::count_if(mItems, RETLAMBDA(Item::Started == it.status));

	for (auto& item : mItems) {
		if (Item::NotLoaded == item.status) {
			_startLoad(item);
			if (--loadCount <= 0) {
				break;
			}
		}
	}
}

void Loader::_startLoad(Item& item) {
	item.status = Item::Started;
	mWorld.AddStreamingLevel(item.level.Get());
}

}}
