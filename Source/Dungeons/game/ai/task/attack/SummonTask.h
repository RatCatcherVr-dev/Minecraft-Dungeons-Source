#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/action/FocusActions.h"
#include "lovika/io/IoMobTypes.h"

class USummonTask : public UBtLeaf {
public:
	USummonTask(
		int maxCount,
		const io::MobGroup&,
		bt::Duration summonDuration,
		bt::Duration summonCooldown,
		const bt::Provider<int>& atATime = bt::value(2),
		const bt::locator::Provider& target = bt::locator::RandomReachablePointAround(bt::location::Self(), 350)
	);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;
	void OnStart(bt::StateRef) override;
private:
	bool CanSpawnMore() const;
	bool TrySummon(bt::StateRef);

	std::vector<io::MobGroup> mobGroups;
	int maxCount;
	int RequestedSpawnCount;
	bt::Provider<int> atATime;
	bt::locator::Provider target;
	bt::Duration summonDuration;
	bt::Duration summonCooldown;
	bt::TimeStamp summonTime;
	bt::TimeStamp earliestStartTime;
	bt::TimeStamp clearRemovedMobsTime;
	TArray<TWeakObjectPtr<class AMobCharacter>> summoned;
};
