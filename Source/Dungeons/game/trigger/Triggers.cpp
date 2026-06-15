#include "Dungeons.h"
#include "Triggers.h"
#include "TriggerUtil.h"
#include "OverlapListener.h"
#include "game/Game.h"
#include "game/level/GameTiles.h"
#include "lovika/Region.h"
#include "lovika/RegionPredicates.h"
#include <Components/PrimitiveComponent.h>
#include <LogMacros.h>

namespace game { namespace trigger {

ExpandType getExpandType(const lovika::Region& region) {
	static const RegionPredicate isDeathRegion = regionpredicates::isDeathTrigger();

	if (isDeathRegion(region)) {
		return ExpandType::Down;
	}
	return ExpandType::Both;
}

Triggers::Triggers(Game& game)
	: mGame(game)
{
	mOverlapListener = mGame.world().SpawnActor<AOverlapListener>();
	mOverlapListener->SetListener(this);

	const auto tagFilter = regionpredicates::hasTag("-force"); // boundary-force, reachable-force

	for (auto&& region : mGame.regionFinder().get("*.*.*")) {
		if (region.type() != regiontype::Trigger) {
			continue;
		}

		if (tagFilter(region)) {
			continue;
		}

		auto box = createCollisionBox(*mOverlapListener, region, getExpandType(region));
		box->SetGenerateOverlapEvents(true);
		box->bMultiBodyOverlap = false;
		mOverlapListener->RegisterBeginOverlap(box);

		mRegionComponentMap.emplace(region, box);
		auto tile = getTile(mGame.tiles(), region);
		mRegionListeners.Add(box, Listeners{ region, tile });
	}
}

Triggers::~Triggers() {
	if (mOverlapListener != nullptr){
		mOverlapListener->Destroy();
	}
}

Triggers::UnregisterKey Triggers::enterRegion(const io::RegionLocator& locator, const RegionFunc& func) {
	return enterRegion(mGame.regionFinder().get(locator), func);
}

Triggers::UnregisterKey Triggers::enterRegion(const std::vector<lovika::Region>& regions, const RegionFunc& func) {
	auto unregisterKey = nextRegistrationKey();
	Registration* registration = nullptr;

	for (auto&& region : regions) {
		if (region.type() != regiontype::Trigger) {
			UE_LOG(LogTemp, Error, TEXT("Region type for '%s' is wrong: %s"), UTF8_TO_TCHAR(region.name().c_str()), UTF8_TO_TCHAR((region.type()).displayName.c_str()));
			continue;
		}
		auto it = mRegionComponentMap.find(region);
		if (it == mRegionComponentMap.end()) {
			UE_LOG(LogTemp, Error, TEXT("Couldn't find trigger for region '%s'! Report to developers"), UTF8_TO_TCHAR(region.name().c_str()));
			continue;
		}
		if (!registration) {
			registration = &mRegistrations.emplace(unregisterKey, Registration{ func }).first->second;
		}
		auto component = it->second;
		mRegionListeners[component].functions.Add(&registration->func);
		registration->components.Add(component);
	}
	return registration ? unregisterKey : UnregisterKey();
}

void Triggers::unregister(UnregisterKey key) {
	if (key.value == 0) {
		return;
	}
	auto registration = mRegistrations.find(key);
	if (registration == mRegistrations.end()) {
		return;
	}
	for (auto&& component : registration->second.components) {
		mRegionListeners[component].functions.Remove(&registration->second.func);
	}
	mRegistrations.erase(registration);
}

void Triggers::onBeginOverlap(const internal::BeginOverlapState& state) {
	auto& listeners = mRegionListeners.FindChecked(&state.regionComponent);

	RegionState regionState{
		listeners.region,
		(ABaseCharacter&)state.actor,
		*listeners.tile
	};

	for (auto&& func : listeners.functions) {
		(*func)(regionState);
	}
}

Triggers::UnregisterKey Triggers::nextRegistrationKey() {
	++mCurrentRegistrationKey.value;
	return mCurrentRegistrationKey;
}

}}
