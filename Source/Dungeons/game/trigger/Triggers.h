#pragma once

#include "OverlapTypes.h"
#include "game/level/GameTile.h"
#include "game/objective/RegionFinder.h"

class ABaseCharacter;
namespace lovika {
	class Region;
}
class AOverlapListener;

namespace io {
using RegionLocator = std::string;
}

namespace game {

class Game;

namespace trigger {

struct RegionState {
	const lovika::Region& region;
	ABaseCharacter& actor;
	TileRef tile;
};

using RegionFunc = std::function<void(const RegionState&)>;

class Triggers {
public:
	class UnregisterKey { friend Triggers; unsigned int value = 0; };

	Triggers(Game&);
	~Triggers();

	UnregisterKey enterRegion(const io::RegionLocator&, const RegionFunc&);
	UnregisterKey enterRegion(const std::vector<lovika::Region>&, const RegionFunc&);
	void unregister(UnregisterKey);
	void onBeginOverlap(const internal::BeginOverlapState&);
private:
	UnregisterKey nextRegistrationKey();

	friend class AOverlapListener;
	struct Listeners {
		lovika::Region region;
		TilePtr tile;
		TArray<RegionFunc*> functions;
	};
	struct Registration {
		RegionFunc func;
		TArray<UPrimitiveComponent*> components;
	};

	Game& mGame;
	TWeakObjectPtr<AOverlapListener> mOverlapListener;
	UnregisterKey mCurrentRegistrationKey;

	struct UnregisterKeyEqHash {
		unsigned int operator()(UnregisterKey key) const { return key.value; };
		bool         operator()(UnregisterKey a, UnregisterKey b) const { return a.value == b.value; };
	};
	// @note: The reason we use STL here (except the laziness) is that
	//        we need well defined reference and iterator invalidations.
	std::unordered_map<lovika::Region, UPrimitiveComponent*> mRegionComponentMap;
	std::unordered_map<UnregisterKey, Registration, UnregisterKeyEqHash, UnregisterKeyEqHash> mRegistrations;
	TMap<UPrimitiveComponent*, Listeners> mRegionListeners;
};

}}
