#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/Actors.h"

class UTeleportTask : public UBtLeaf {
public:
	UTeleportTask(
		bt::Duration teleportOutDuration,
		bt::Duration teleportInDuration,
		const bt::Provider<bt::Duration>& disappearDurationProvider,
		const bt::locator::Provider& target,
		const bt::locator::Provider& lookAtTarget
	);

protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;
	
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;	

private:
	bt::Duration teleportOutDuration;
	bt::Duration teleportInDuration;

	bt::Provider<bt::Duration> disappearDurationProvider;
	bt::Duration disappearDuration;
	
	bt::TimeStamp disableTargetTime;
	bt::TimeStamp disappearTime;
	bt::TimeStamp teleportInTime;
	bt::TimeStamp completeTime;	
	
	bt::locator::Provider target;
	bt::locator::Provider lookAtTarget;
	
	FVector teleportInLocation;	

	bool disableTargetDone;
	bool disappearDone;
	bool teleportDone;

	TOptional<FVector> FindTeleportInLocation(bt::StateRef state) const;
};
