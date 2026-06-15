#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"
#include "game/actor/EffectsActor.h"
#include "Animation/AnimSequenceBase.h"

class USummonFormationTask : public UBtLeaf {
public:
	struct SummonEntry {
		bt::Duration delay;
		EntityType type;
		TSubclassOf<AActor> subclass;
		FTransform transform;
		bool ignorePlayers;
		bool transferEnchantments;
	};

	struct ParticleEntry {
		bt::Duration delay;
		FTransform transform;
	};

	using AddEntryRef = const std::function<void (const bt::Duration&, const EntityType&, const TSubclassOf<AActor>&, const FTransform&, bool, bool)>&;
	using SummonRoutine = const std::function<void (bt::StateRef, const SummonEntry&)>;

	USummonFormationTask(
		UAnimSequenceBase* sequence,
		bt::Duration animationDuration,
		bt::Duration summonStart,
		bt::Duration summonCooldown,
		bt::Duration disappearDelay,
		bt::Duration appearDelay,
		const std::function<void (bt::StateRef, AddEntryRef)>& generateEntries,
		SummonRoutine& summon = USummonFormationTask::SummonMob,
		bt::Duration emergeDuration = 0s
	);

	static void SummonMob(bt::StateRef, const SummonEntry&);
	static void SummonMobNoAdjustPosition(bt::StateRef, const SummonEntry&);
	static void SummonMobWithDrops(bt::StateRef, const SummonEntry&);
	static void SummonMobUnderling(bt::StateRef, const SummonEntry&);
	static void SummonActor(bt::StateRef, const SummonEntry&);
	static struct FGameplayEffectSpecHandle GenerateTransferSpec(bt::StateRef);

protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void Init(bt::StateRef) override;
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;

private:
	bt::locator::Provider target;
	UAnimSequenceBase* sequence;
	bt::Duration animationDuration;
	bt::Duration summonStart;
	bt::Duration summonCooldown;
	bt::Duration disappearDelay;
	bt::Duration appearDelay;
	bt::Duration emergeDuration;
	std::function<void(bt::StateRef, AddEntryRef)> generateEntries;
	std::function<void(bt::StateRef, const SummonEntry&)> summon;
	//bt::TimeStamp startTime;
	float startTime;
	bt::TimeStamp summonStartTime;
	bt::TimeStamp animationEndTime;
	bt::TimeStamp earliestStartTime;
	
	bt::TimeStamp disappearTime;
	bt::TimeStamp appearTime;
	bool disappearDone;
	bool appearDone;
		
	std::queue<SummonEntry> summonQueue;
	std::queue<ParticleEntry> particleQueue;

	bool hasSummoned = false;

	TWeakObjectPtr<AEffectsActor> effectsActor;
};
