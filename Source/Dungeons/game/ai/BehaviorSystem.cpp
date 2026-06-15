#include "Dungeons.h"
#include "BehaviorSystem.h"
#include "bt/BtEvalState.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/BehaviorComponent.h"
#include "game/component/MobCharacterMovementComponent.h"
#include "world/entity/MobTags.h"
#include <EngineUtils.h>
#include "util/Algo.h"


#define SYNC_UPDATE_BEHAVIOUR 1/*(PLATFORM_SWITCH)*/

DECLARE_CYCLE_STAT(TEXT("UBehaviorSystem::Update"), STAT_UBehaviorSystem_Update, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("UBehaviorSystem::Update Sync"), STAT_UBehaviorSystem_UpdateSync, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("BehaviorSystem::CompleteWillRunAsyncTasks"), STAT_BehaviorSystem_CompleteWillRunAsyncTasks, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("UpdateFromWillRun"), STAT_BehaviorSystem_UpdateFromWillRun , STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_NonTarget"), STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_NonTarget, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_Target"), STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_Target, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BehaviorSystem_CompleteCurrentWillRunTasks_TargetInternal"), STAT_BehaviorSystem_CompleteCurrentWillRunTasks_TargetInternal, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BehaviorSystem_CompleteCurrentWillRunTasks_NonTargetInternal"), STAT_BehaviorSystem_CompleteCurrentWillRunTasks_NonTargetInternal, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BehaviorSystem_CompleteCurrentWillRunTasks_NonTarget"), STAT_BehaviorSystem_CompleteCurrentWillRunTasks_NonTarget, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BehaviorSystem_CompleteCurrentWillRunTasks_Target"), STAT_BehaviorSystem_CompleteCurrentWillRunTasks_Target, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_BuildInclusionPredicates"), STAT_BuildInclusionPredicates, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_SetMobEnabled_true"), STAT_SetMobEnabled_true, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_SetMobEnabled_false"), STAT_SetMobEnabled_false, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_GetBehaviorMobs"), STAT_GetBehaviorMobs, STATGROUP_AI);



UBehaviorSystem::UBehaviorSystem(UWorld& world)
	: mWorld(world)
	, mTimer(bt::Duration::TicksPerSecond, [&world]() { return static_cast<int>(1000 * world.GetTimeSeconds()); }, 1)
	, mTickId(0) 
{
#if !SYNC_UPDATE_BEHAVIOUR
	//Create a pool of async updates to pull from	
	int i(FMath::Min(FPlatformMisc::NumberOfWorkerThreadsToSpawn(), (int32)FBehaviorSystemRunnable::EBehaviorSystemThreadPoolMax));

	while (i--)
	{
		mBehaviourSystemThreads.Push(new FBehaviorSystemRunnable());
	}
#endif

}

UBehaviorSystem::~UBehaviorSystem()
{
	for (auto BehaviourSystemThread : mBehaviourSystemThreads)
	{
		delete BehaviourSystemThread;
	}
}

TAutoConsoleVariable<int32> CVarPauseFarMobsMeters(
	TEXT("Dungeons.Mob.AiPauseFar"),
	70,
	TEXT("Pauses AI on mobs that are further than a specified block distance away.\n")
	TEXT("<= 0: off (no pause).\n")
	TEXT(">  0: pause AI at a given block distance.\n"),
	ECVF_Cheat
);

namespace {

using ActorPred = Pred<const AActor*>;

struct RelevantMobs {
	TArray<AMobCharacter*> relevant;
	TArray<AMobCharacter*> irrelevant;
};

RelevantMobs GetBehaviorMobs(UWorld& world, const std::vector<ActorPred>& inclusionPredicates) {
	SCOPE_CYCLE_COUNTER(STAT_GetBehaviorMobs);
	RelevantMobs out;
	for (auto* mob : InstanceTracker<AMobCharacter>::GetList(&world)) {
		if (mob->Behavior && (inclusionPredicates.empty() || algo::any_of(inclusionPredicates, RETLAMBDA(it(mob))))) {
			out.relevant.Add(mob);
		} else {
			out.irrelevant.Add(mob);
		}
	}
	return out;
}

std::vector<ActorPred> BuildInclusionPredicates(UWorld& world, float maxDistanceMeters) {
	SCOPE_CYCLE_COUNTER(STAT_BuildInclusionPredicates);
	if (maxDistanceMeters <= 0) {
		return {};
	}
	const float maxDistanceUnits = maxDistanceMeters * Math::PE_TO_UE_UNITS;
	return algo::map_vector(InstanceTracker<APlayerCharacter>::GetList(&world), RETLAMBDA(actorquery::is::inRange(it->GetActorLocation(), maxDistanceUnits)));
}

void SetMobEnabled(AMobCharacter& mob, bool enabled) {
	if (hasMobTag(mob.EntityType, MobTags::HashTag_Miniboss)) {
		return;
	}
	if (auto* movement = Cast<UMobCharacterMovementComponent>(mob.GetMovementComponent())) {
		movement->SetComponentTickEnabled(enabled);
	}
}

}

void UBehaviorSystem::Update() {

#if SYNC_UPDATE_BEHAVIOUR
	Update_Sync();
#else
	Update_ASync();
#endif //SYNC_UPDATE_BEHAVIOUR
		
	
}

void UBehaviorSystem::Update_ASync()
{
	SCOPE_CYCLE_COUNTER(STAT_UBehaviorSystem_Update);

	if (mPaused)
	{
		return;
	}

	mTimer.advanceTime();

	const int ticks = mTimer.getTicks();
	if (ticks == 0) {
		return;
	}

	const auto predicates = BuildInclusionPredicates(mWorld,
		CVarPauseFarMobsMeters.GetValueOnGameThread());

	const auto mobs = GetBehaviorMobs(mWorld, predicates);
	for (auto* mob : mobs.relevant) { SetMobEnabled(*mob, true); }
	for (auto* mob : mobs.irrelevant) { SetMobEnabled(*mob, false); }

	TArray<AMobCharacter *> relevantWithUpdate;
	relevantWithUpdate.Reserve(mobs.relevant.Num());

	//do pre-checks so we only process the ones we need
	for (auto* mob : mobs.relevant)
	{
		if (ShouldTickMob(*mob))
		{
			relevantWithUpdate.Push(mob);
		}
	}

	//early out
	const int iMobCount = relevantWithUpdate.Num();
	if (!iMobCount)
	{
		mTickId += ticks;
		return;
	}

	const int iNumThreads = mBehaviourSystemThreads.Num();
	
	for (int i = 0; i < ticks; ++i)
	{
		++mTickId;
		bt::internal::currentTickId = mTickId;

		//create all current states for this tick
		for (auto mob : relevantWithUpdate)
		{
			mob->Behavior->CreateCurrentBehaviourEvalState(*mob, mTickId);
		}

		{
			//Perform Async Willrun Checks and Main thread Tick Updates for Behaviour Target Tree

			//kick off async willrun tests for Targets
			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_Target);

				FPlatformMisc::MemoryBarrier();

				for (auto BehaviourSystemThread : mBehaviourSystemThreads)
				{
					BehaviourSystemThread->SetTargetMode(true);
				}

				for (int iMob = 0; iMob < iMobCount; ++iMob)
				{
					mBehaviourSystemThreads[iMob % iNumThreads]->AddProcessMob(relevantWithUpdate[iMob]);
				}
			}
		

			//stall waiting for all the mob tasks to complete
			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_CompleteCurrentWillRunTasks_Target);

				//wait for threads to finish
				for (auto BehaviourSystemThread : mBehaviourSystemThreads)
				{
					SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_CompleteCurrentWillRunTasks_TargetInternal);
					while (!BehaviourSystemThread->CompletedProcessingMobs())
					{
						FPlatformProcess::SleepNoStats(0);
					}
				}
			}

			FPlatformMisc::MemoryBarrier();

			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_UpdateFromWillRun);
				//run the update to process any nodes that were stored by the WillrunCheck for target supdate
				for (auto mob : relevantWithUpdate)
				{
					mob->Behavior->UpdateFromWillRunChecks(true);
				}
			}

		}

		FPlatformMisc::MemoryBarrier();

		{

			//Perform Async Willrun Checks and Main thread Tick Updates for Behaviour Action Tree
			
			//kick off async willrun tests for actions
			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_NonTarget);

				FPlatformMisc::MemoryBarrier();
				
				for (auto BehaviourSystemThread : mBehaviourSystemThreads)
				{
					BehaviourSystemThread->SetTargetMode(false);
				}

				for (int iMob = 0; iMob < iMobCount; ++iMob)
				{
					mBehaviourSystemThreads[iMob % iNumThreads]->AddProcessMob(relevantWithUpdate[iMob]);
				}
				
			}

			//stall waiting for all the mob tasks to complete
			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_CompleteCurrentWillRunTasks_NonTarget);
				
				//wait for threads to finish
				for (auto BehaviourSystemThread : mBehaviourSystemThreads)
				{
					SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_CompleteCurrentWillRunTasks_NonTargetInternal);
					while (!BehaviourSystemThread->CompletedProcessingMobs())
					{
						FPlatformProcess::SleepNoStats(0);
					}
				}

			}

			FPlatformMisc::MemoryBarrier();

			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_UpdateFromWillRun);
				//run the update to process any nodes that were stored by the WillrunCheck
				for (auto mob : relevantWithUpdate)
				{
					mob->Behavior->UpdateStunnedState();
					mob->Behavior->UpdateBubbledState();
					mob->Behavior->UpdateFromWillRunChecks(false);
					mob->Behavior->ResetBehaviorCountDown();
				}
			}

		}

	}
}

void UBehaviorSystem::Update_Sync()
{
	SCOPE_CYCLE_COUNTER(STAT_UBehaviorSystem_UpdateSync);

	if (mPaused)
	{
		return;
	}

	mTimer.advanceTime();

	const int ticks = mTimer.getTicks();
	if (ticks == 0) {
		return;
	}

	const auto predicates = BuildInclusionPredicates(mWorld,
		CVarPauseFarMobsMeters.GetValueOnGameThread());

	const auto mobs = GetBehaviorMobs(mWorld, predicates);
	for (auto* mob : mobs.relevant) { SetMobEnabled(*mob, true); }
	for (auto* mob : mobs.irrelevant) { SetMobEnabled(*mob, false); }

	TArray<AMobCharacter *> relevantWithUpdate;
	relevantWithUpdate.Reserve(mobs.relevant.Num());

	//do pre-checks so we only process the ones we need
	for (auto* mob : mobs.relevant)
	{
		if (ShouldTickMob(*mob))
		{
			relevantWithUpdate.Push(mob);
		}
	}

	//early out
	const int iMobCount = relevantWithUpdate.Num();
	if (!iMobCount)
	{
		mTickId += ticks;
		return;
	}

	
	for (int i = 0; i < ticks; ++i)
	{
		++mTickId;
		bt::internal::currentTickId = mTickId;

		//create all current states for this tick
		for (auto mob : relevantWithUpdate)
		{
			mob->Behavior->CreateCurrentBehaviourEvalState(*mob, mTickId);
		}


		{
			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_Target);
				for (auto mob : relevantWithUpdate)
				{
					mob->Behavior->PerformWillRunChecks(true);
				}
			}

			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_UpdateFromWillRun);
				//run the update to process any nodes that were stored by the WillrunCheck for target supdate
				for (auto mob : relevantWithUpdate)
				{
					mob->Behavior->UpdateFromWillRunChecks(true);
				}
			}
		}

		{
			
			//kick off willrun tests for actions
			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_FMobBehaviourUpdateWillRunTask_NonTarget);
				for (auto mob : relevantWithUpdate)
				{
					mob->Behavior->PerformWillRunChecks(false);
				}

			}

			{
				SCOPE_CYCLE_COUNTER(STAT_BehaviorSystem_UpdateFromWillRun);
				//run the update to process any nodes that were stored by the WillrunCheck
				for (auto mob : relevantWithUpdate)
				{
					mob->Behavior->UpdateStunnedState();
					mob->Behavior->UpdateBubbledState();
					mob->Behavior->UpdateFromWillRunChecks(false);
					mob->Behavior->ResetBehaviorCountDown();
				}
			}
		}

	}
}


void UBehaviorSystem::SetPaused(bool pause) {
	mPaused = pause;
}


bool UBehaviorSystem::ShouldTickMob(class AMobCharacter& mob)
{
	if (mob.mExtremeRangeFromPlayers) //D11.SC dont tick mob behavior at extreme ranges
	{
		return false;
	}
	if (!mob.GetController()) { // @todo: fix this nicer, please. We UnPossess on mob death, but we might tick BT a few more ticks (in the same frame)
		return false;
	}
	if (!mob.Behavior->IsEnabled()) {
		return false;
	}
	if (mob.IsPendingKillPending()) { // @btlifecycle, @remove?
		mob.Behavior->Clear();
		return false;
	}
	if (!mob.Behavior->BehaviorCountDown()) { //count down?
		return false;
	}

	return true;
}


void FBehaviorSystemRunnable::AddProcessMob(AMobCharacter* pMob) {
	mTargetMobQueue.Enqueue(pMob);
	mEvent->Trigger();
}

uint32 FBehaviorSystemRunnable::Run()
{
	while (mRunning)
	{
		AMobCharacter* ProcessMob = nullptr;
		
		while (mTargetMobQueue.Peek(ProcessMob))
		{
			ProcessMob->Behavior->PerformWillRunChecks(mTargetMode);
			mTargetMobQueue.Pop();
		}

		mEvent->Wait(FTimespan(0, 0, 0, 1, 0));
	}
	return 0;
}
