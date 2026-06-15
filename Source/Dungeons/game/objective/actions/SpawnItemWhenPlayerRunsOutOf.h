#pragma once

#include "game/objective/ObjectiveAction.h"
#include "lovika/Region.h"
#include <string>
#include "game/item/ItemType.h"
#include "Engine/StreamableManager.h"

class ItemType;

namespace io { using RegionLocator = std::string; }

namespace game { namespace objective {

class SpawnItemWhenPlayerRunsOutOf: public ObjectiveAction {
public:
	SpawnItemWhenPlayerRunsOutOf(const io::RegionLocator&, const FItemId&);
	~SpawnItemWhenPlayerRunsOutOf();
	void onInit() override;
	void onTick() override;
private:
	io::RegionLocator mLocator;
	const ItemType& mItem;
	std::vector<lovika::Region> mRegions;
	TArray<TWeakObjectPtr<AActor>> mSpawnedItems;
	TSharedPtr< FStreamableHandle > ItemClassStreamingHandle;
};

}}
