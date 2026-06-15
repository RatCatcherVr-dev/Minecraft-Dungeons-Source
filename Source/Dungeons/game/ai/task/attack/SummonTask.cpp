#include "Dungeons.h"
#include "SummonTask.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/component/BaseParticleAssetsComponent.h"
#include "game/component/MobAnimationsComponent.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/util/Tags.h"
#include "SummonFormationTask.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/GameBP.h"

USummonTask::USummonTask(
	int maxCount,
	const io::MobGroup& mobGroup,
	bt::Duration summonDuration,
	bt::Duration summonCooldown,
	const bt::Provider<int>& atATime /*= bt::value(2)*/,
	const bt::locator::Provider& target /*= bt::locator::RandomReachablePointAround(bt::location::Self(), 350)*/
)
	: maxCount(maxCount)
	, RequestedSpawnCount(0)
	, summonDuration(summonDuration)
	, summonCooldown(summonCooldown)
	, atATime(atATime)
	, target(target)
{
	name = "summon-task";

	mobGroups.push_back(mobGroup);

	for (auto&& group : mobGroups) {
		game::mobspawn::prepareMobGroup(group);
	}
}

bool USummonTask::OnCanRun(bt::StateRef state) {
	if (!earliestStartTime.IsPassed(state)) {
		return false;
	}
	if (clearRemovedMobsTime.IsPassed(state)) {
		summoned.RemoveAll([](auto mob) { return mob == nullptr || (mob.IsValid() && !mob->IsAlive()); });
		clearRemovedMobsTime = bt::TimeStamp::FromNow(state, 1s);
	}
	return CanSpawnMore();
}

bool USummonTask::OnCanContinue(bt::StateRef state) {
	if (!summonTime.IsPassed(state)) {
		return true;
	}
	if (TrySummon(state)) {
		earliestStartTime = bt::TimeStamp::FromNow(state, summonCooldown);
	}

	return false;
}

void USummonTask::OnStart(bt::StateRef state) {
	summonTime = bt::TimeStamp::FromNow(state, summonDuration);
	if (const auto animPack = state.animPack()) {
		if (auto sequence = animPack->Common.BasicAttack) {
			state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("UpperBody")), 0.f, 0.2f, 1.f, 1, 0.f, 0.f, FPredictionKey());
			state.owner->RemoveInvisibility();
		}
	}
}

bool USummonTask::CanSpawnMore() const {
	return (summoned.Num() + RequestedSpawnCount) < maxCount;
}

bool USummonTask::TrySummon(bt::StateRef state) 
{
	if (CanSpawnMore())
	{
		AGameBP* game = actorquery::getFirstActor<AGameBP>(state.owner->GetWorld());

		TWeakObjectPtr<AMobCharacter> weakOwnerPtr = state.owner;

		auto specHandle = USummonFormationTask::GenerateTransferSpec(state);

		for (auto&& entityType : game::mobspawn::calculateMobsWithoutDifficultyLimitation(mobGroups, atATime(state))) {
			if (!CanSpawnMore()) {
				break;
			}

			if (auto pos = target(state).GetLocationLike())
			{
				const auto spawnConfig = game::mobspawn::configs::Default(state.difficulty(), false).AddTag(tags::noDrop);
				FTransform MobTransform;

				if (!game::mobspawn::getSpawnData(*state.owner->GetWorld(), game::mobspawn::providers::Location(pos.GetValue()), spawnConfig, entityType, MobTransform))
				{
					continue;
				}

							
				++RequestedSpawnCount;
				
				//request a spawn with callback
				game->RequestMobSpawn(entityType, MobTransform, {}, spawnConfig, [this, weakOwnerPtr, specHandle](AMobCharacter* mob) {
					if(mob) {
						mob->GetAbilitySystemComponent()->BP_ApplyGameplayEffectSpecToSelf(specHandle);
					}
					
					// This is touching undefined behaviour, but hopefully it's OK.
					// We can test it and later, if this doesn't work, do a more cumbersome fix.
					if (weakOwnerPtr.IsValid() && weakOwnerPtr->IsAlive()) {

						--RequestedSpawnCount;

						if (mob) {
							if (weakOwnerPtr->Particles && weakOwnerPtr->Particles->Summon) {
								mob->SpawnAttachedParticles(weakOwnerPtr->Particles->Summon);
							}
							summoned.Add(mob);
						}
					}
				});
			}

		}

		return true;
	}

	return false;
}
