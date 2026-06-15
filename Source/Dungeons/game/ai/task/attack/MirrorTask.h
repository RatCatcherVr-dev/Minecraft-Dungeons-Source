#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/Actors.h"
#include "game/mobspawn/MobSpawnTypes.h"

class UMirrorTask : public UBtLeaf {
public:
	UMirrorTask(
		bt::Duration teleportOutDuration,
		bt::Duration teleportInDuration,
		int count,
		float circleRadiusMax,
		float anchorDistanceMax,
		float mirrorDistanceMax,
		const bt::actor::Provider& target,
		const bt::location::Provider& anchor
	);

protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;
	
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;	
private:
	bt::Duration teleportOutDuration;
	bt::Duration teleportInDuration;
	bt::TimeStamp teleportInTime;
	bt::TimeStamp disableTargetTime;
	bt::TimeStamp completeTime;
	int count;
	float circleRadiusMax;
	float anchorDistanceMax;
	float mirrorDistanceMaxSquare;
	bt::actor::Provider target;
	bt::location::Provider anchor;
	game::mobspawn::Config spawnConfig;

	FVector teleportInLocation;
	bool disableTargetDone;
	bool teleportDone;
};
