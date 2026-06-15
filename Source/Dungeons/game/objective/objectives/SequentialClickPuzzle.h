#pragma once

#include "game/objective/Objective.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/mobspawn/SpawnRegions.h"
#include "game/level/GameTile.h"
#include "lovika/Region.h"
#include "lovika/io/IoObjectiveTypes.h"
#include <Array.h>
#include <WeakObjectPtrTemplates.h>
#include <vector>

namespace game { namespace objective {

class DoorLocker;
class SpawnQueue;

class SequentialClickPuzzle : public Objective {
public:
	SequentialClickPuzzle(const io::ObjectiveClickData&);
	~SequentialClickPuzzle();

	Validation validate(ValidationType) const;

	FObjectiveLocations getLocations() const override;
protected:
	void onInit() override;
	void onStart() override;
	void onTick() override;
	void onStop() override;
private:
	void _onClickedObject(AActor&);
	void _onClickedObjectOnce(AActor&);

	EntityType keyType() const;

	struct Positions { TArray<Vec3> objects, dummies; };
	Positions generatePositions(size_t count, Random&) const;
	TOptional<std::vector<io::RegionLocator>> _getSpawnRegionLocators() const;

	bool _spawnKeyIfWanted(class Random&);

	io::ObjectiveClickData mData;
	std::vector< lovika::Region> mLocations;

	struct Object {
		TWeakObjectPtr<AActor> actor;
		TWeakObjectPtr<UInteractableComponent> interactable;
		bool clicked;
	};
	TArray<Object> mObjects;

	int indexOf(const AActor&) const;
	void tryActivateObjectAtIndex(int) const;

	Unique<DoorLocker> mDoorLocker;
	mobspawn::Regions mSpawnRegions;
	TWeakObjectPtr<AMobCharacter> mKeyMob;

	bool firstPickup;
	Unique<SpawnQueue> mSpawnQueue;
	TArray<TWeakObjectPtr<AMobCharacter>> mMobs;
};

}}
