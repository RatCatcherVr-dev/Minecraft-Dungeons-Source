#pragma once

#include "game/objective/Objective.h"
#include "game/objective/ObjectiveDoorLocker.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/level/GameTile.h"
#include "lovika/io/IoObjectiveTypes.h"

namespace game { namespace objective {

class KillGroup : public Objective {
public:
	KillGroup(const io::ObjectiveKillGroupData&);

	Validation validate(ValidationType) const override;

	FObjectiveLocations getLocations() const override;
protected:
	void onInit() override;
	void onTick() override;
	void onStart() override;
	void onStop() override;
private:
	TilePtr getTargetTile() const;
	mobspawn::Regions getSpawnRegions() const;

	struct FSharedKillGroupMobSpawndata
	{
		int mRequestedMobSpawns = 0;
		FVector mLastValidMobPosition = FVector::ZeroVector;
		TArray<TWeakObjectPtr<AMobCharacter>> mMobTargetGroup;
	};

	TSharedPtr<FSharedKillGroupMobSpawndata> mKillGroupMobSpawnData;
	
	io::ObjectiveKillGroupData mData;

	TilePtr mTargetTile;
	Unique<class DoorLocker> mDoorLocker;

	FObjectiveLocations mMarkerLocations;
	TArray<io::MobGroup> mMobGroups;	
	
};

}}
