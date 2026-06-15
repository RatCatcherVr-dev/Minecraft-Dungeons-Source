#include "Dungeons.h"
#include "WickedWraithBehavior.h"

#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WeakenGameplayEffect.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/Move.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/component/AoeAttackComponent.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "util/FloatRange.h"
#include "util/Random.h"
#include "util/RandomUtil.h"

UWickedWraithVulnerableGameplayEffect::UWickedWraithVulnerableGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo resistDeathInfo;
	resistDeathInfo.Attribute = UHealthAttributeSet::ResistDeathAttribute();
	resistDeathInfo.ModifierMagnitude = FScalableFloat( 1.0f );
	resistDeathInfo.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(resistDeathInfo);
}

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createWickedWraith(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		static Random rnd;

		auto& aoeComponent = *mob.FindComponentByClass<UAoeAttackComponent>();
		auto& rangeComponent = *mob.FindComponentByClass<URangedAttackComponent>();

		auto abilitySystem = mob.GetAbilitySystemComponent();
		if (abilitySystem) {
			auto spec = effects::CreateGameplayEffectSpec<UWickedWraithVulnerableGameplayEffect>(abilitySystem);
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}

		const auto optAOERange = aoeComponent.GetAttackRange();
		const auto optAOECooldown = options.Get("aoe-cooldown", 7.0s);

		const auto optWaypointWaitTime = options.Get("waypoint-wait-time", 5.0s);
		const auto optWaypointMaxDistance = options.Get("waypoint-max-distance", 1500.0);
		const auto optWaypointMinDistance = options.Get("waypoint-min-distance", 300.0);
		
		const auto optTeleportInDuration = options.Get("teleport-in-duration", 1.0s);
		const auto optTeleportOutDuration = options.Get("teleport-out-duration", 1.0s);
		const auto optTeleportDuration = options.Get("teleport-duration", 1.0s);

		const auto optSummonRadius = options.Get("summon-radius", 1500.0f);
		const auto optSummonMinStrayCount = options.Get("summon-min-stray-count", 2.0f);
		const auto optSummonMaxStrayCount = options.Get("summon-max-stray-count", 3.0f);
		const auto optSummonMinWraithCount = options.Get("summon-min-wraith-count", 2.0f);
		const auto optSummonMaxWraithCount = options.Get("summon-max-wraith-count", 3.0f);
		const auto optSummonSpawnRate = options.Get("summon-spawn-rate", 1.0f);

		const auto optDanmakuHealthTrigger = options.Get("danmaku-health-trigger", 0.5f);
		const auto optDanmakuDuration = options.Get("danmaku-duration", 10.0s);
		const auto optDanmakuCooldown = options.Get("danmaku-cooldown", 20.0s);
		const auto optDanmakuAnimBegin = options.Get("danmaku-anim-begin", 1.0s);
		const auto optDanmakuAnimEnd = options.Get("danmaku-anim-end", 1.0s);

		const auto optWeakenedAnimBegin = options.Get("weakened-anim-begin", 1.0s);
		const auto optWeakenedAnimEnd = options.Get("weakened-anim-end", 1.0s);
		const auto optWeakenedDuration = options.Get("weakened-duration", 7.0s);
		const auto optWeakenedDamageBoost = options.Get("weakened-damage-boost", 4.0f);
		const auto optWeakenedHealthDelta = options.Get("weakened-health-delta", 0.2f);
		const auto optWeakenedDeath = options.Get("weakened-health-death", 0.02f);

		const auto varWaypoints = sharedRef(actorquery::getActors<AWickedWraithWaypoint>(mob.GetWorld()));
		const auto varCanSummon = sharedRef(true);
		const auto varLastWeakenedHealth = sharedRef(1.0f);

		const auto getNextWeakenedHealth = [=]() -> bt::Provider<float> {
			return [=](StateRef state) {
				return FMath::Max(optWeakenedDeath, *varLastWeakenedHealth - optWeakenedHealthDelta);
			};
		};


		const auto getRandomLocation = [=,&mob](EWickedWraithWaypointType type, float minDistance = 0.0f, float maxDistance = 5000.0f) {
			return [=, &mob](StateRef state) {
				auto waypointCheck = [type, maxRadiusSq = maxDistance * maxDistance, minRadiusSq = minDistance * minDistance, &mob](AWickedWraithWaypoint* wp) {
					const float distSq = FVector::DistSquared2D(mob.GetActorLocation(), wp->GetActorLocation());
					return wp->WaypointType == type && distSq > minRadiusSq && distSq < maxRadiusSq;
				};
				auto nearbyWaypoints = varWaypoints->FilterByPredicate(waypointCheck);
				if (auto waypoint = Util::randomChoiceOrEmpty(nearbyWaypoints)) {
					return waypoint.GetValue()->GetActorLocation();
				}
				return mob.GetActorLocation();
			};
		};

		const auto randomMoveRequest = [=](float minDistance, float maxDistance) {
			return bt::move::Location(getRandomLocation(EWickedWraithWaypointType::Normal, minDistance, maxDistance));
		};

		const auto setRangeAttackVariant = [&rangeComponent](int index) {
			return [&rangeComponent,index](StateRef state) {
				rangeComponent.SetRangeAttackVariant(index);
			};
		};

		const auto resetRangeAttackVariant = [&rangeComponent]() {
			return [&rangeComponent](StateRef state) {
				rangeComponent.ResetRangeAttackVariant();
			};
		};

		const auto teleportDuration = [=](StateRef state) {
			return optTeleportDuration;
		};

		const auto teleportLocation = [=](StateRef state) {
			return getRandomLocation(EWickedWraithWaypointType::Normal)(state);
		};

		const auto teleportLocationSpecial = [=](StateRef state) {
			return getRandomLocation(EWickedWraithWaypointType::Special)(state);
		};

		const auto attackRanged = [=,&mob]() {
			return rangedAttack(mob, 0.0f, 10000.0f, 20000.0f);
		};

		const auto summonFormation = [=] ( bt::StateRef state, USummonFormationTask::AddEntryRef addEntry ) {
			const auto strayCount = rnd.nextInt( optSummonMinStrayCount, optSummonMaxStrayCount );
			const auto wraithCount = rnd.nextInt( optSummonMinWraithCount, optSummonMaxWraithCount );
			float minDelay = 0.2f * optSummonSpawnRate;
			float maxDelay = 0.8f * optSummonSpawnRate;

			float delay = 0.0f;
			for( int i = 0; i < strayCount; ++i ) {
				if( auto location = locationquery::getRandomLocationAround(state.world(), state.owner->GetActorLocation(), optSummonRadius) ) {
					addEntry(bt::Seconds(delay), EntityType::Stray, nullptr, FTransform( location.GetValue() ), true, false);
				}
				delay += rnd.nextFloat( minDelay, maxDelay );
			}
			delay = 0.2f;
			for( int i = 0; i < wraithCount; ++i ) { 
				if( auto location = locationquery::getRandomLocationAround(state.world(), state.owner->GetActorLocation(), optSummonRadius) ) {
					addEntry(bt::Seconds(delay), EntityType::Wraith, nullptr, FTransform( location.GetValue() ), true, false);
				}
				delay += rnd.nextFloat( minDelay, maxDelay );
			}
		};

		const auto attackDanmaku = [=,&options]() {
			return uninterruptibleSequence("attackDanmaku",
				startPredicate(lessThan(actorStats::healthFraction(), value(optDanmakuHealthTrigger))),
				onStop(resetRangeAttackVariant()),
				minTimeBetweenStopAndStart(optDanmakuCooldown),
				chain(
					make_unique<UTeleportTask>(
						optTeleportOutDuration,
						optTeleportInDuration,
						teleportDuration,
						teleportLocationSpecial,
						actor::Target()
					),
					sequence(
						alwaysTrue(playAnimation(options.Get("DanmakuBegin"))),
						delay(optDanmakuAnimBegin)
					),
					sequence(
						predicate(greaterThan(actorStats::healthFraction(), value(optWeakenedDeath))),
						alwaysTrue( playAnimation( options.Get( "DanmakuLoop" ), true ) ),
						randomSelector( "attack-pattern",
							sequence(
								alwaysTrue(chain(
									sequence( setRangeAttackVariant(0), attackRanged() ),
									sequence( setRangeAttackVariant(1), attackRanged() ),
									sequence( setRangeAttackVariant(1), attackRanged() )
								)),
								delay(optDanmakuDuration)
							),
							sequence(
								alwaysTrue(chain(
									sequence( setRangeAttackVariant(2), attackRanged() )
								)),
								delay(optDanmakuDuration)
							)
						)
					),
					sequence(
						alwaysTrue(playAnimation(options.Get("DanmakuEnd"))),
						delay(optDanmakuAnimEnd)
					)
				)
			);
		};

		const auto attackSummon = [=]() {
			return uninterruptibleSequence( "attackSummon",
				startPredicate( equals( varCanSummon, value( true ) ) ),
				onStop( set( varCanSummon, value(false) ) ),
				sequence(
					make_unique<USummonFormationTask>( nullptr,
						0.0s, 0.0s, 0.0s, 0.0s, 0.0s,
						summonFormation,
						USummonFormationTask::SummonMob,
						0.0s
					)
				)
			);
		};

		const auto attackAoe = [=]() {
			return uninterruptibleSequence("attackAoe",
				startPredicate(locator::IsInRange(actor::Target(), optAOERange)),
				minTimeBetweenStopAndStart( optAOECooldown ),
				sequence(
					chain(
						std::make_unique<UAoeAttack>(),
						alwaysTrue()
					),
					make_unique<UTeleportTask>(
						optTeleportOutDuration,
						optTeleportInDuration,
						teleportDuration,
						teleportLocation,
						actor::Target()
					)
				)
			);
		};

		const auto roam = [=]() {
			return uninterruptibleSequence(
				startPredicate(!actor::HasRecentlyTeleported(optWaypointWaitTime)),
				minTimeBetweenStopAndStart(optWaypointWaitTime),
				sequence(
					behavior::moveTo(randomMoveRequest(optWaypointMinDistance, optWaypointMaxDistance), Relative(1.0f)),
					delay(3.0s) // Abort after 3 seconds as the moveTo can get stuck.
				)
			);
		};
		
		const auto applyVulnerable = [=] () -> bt::Action {
			return [=](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					FGameplayEffectQuery query;
					query.EffectDefinition = UWickedWraithVulnerableGameplayEffect::StaticClass();
					abilitySystem->RemoveActiveEffects(query);

					auto spec = effects::CreateGameplayEffectSpec<UWeakenGameplayEffect>(abilitySystem);
					spec.SetSetByCallerMagnitude( UWeakenGameplayEffect::DamageTakenName, optWeakenedDamageBoost );
					spec.SetSetByCallerMagnitude( UWeakenGameplayEffect::DurationName, optWeakenedDuration.Seconds() );
					abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
				}
			};
		};

		const auto removeVulnerable = [=]() -> bt::Action {
			return [=](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					auto spec = effects::CreateGameplayEffectSpec<UWickedWraithVulnerableGameplayEffect>(abilitySystem);
					abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

					FGameplayEffectQuery query;
					query.EffectDefinition = UWeakenGameplayEffect::StaticClass();
					abilitySystem->RemoveActiveEffects(query);
				}
			};
		};

		const auto weakened = [=,&options]() {
			return uninterruptibleSequence(
				startPredicate(lessThan(actorStats::healthFraction(), getNextWeakenedHealth())),
				onStop(set(varLastWeakenedHealth, actorStats::healthFraction())),
				chain(
					sequence(
						alwaysTrue(playAnimation(options.Get("WeakenedBegin"))),
						delay(optWeakenedAnimBegin)
					),
					sequence(
						onStart(applyVulnerable()),
						onStop(removeVulnerable()),
						alwaysTrue(playAnimation(options.Get("WeakenedLoop"), true)),
						parallel(
							delay(optWeakenedDuration),
							predicate(lessThan(actorStats::healthFraction(), value(optWeakenedDeath)))
						)
					),
					sequence(
						alwaysTrue(playAnimation(options.Get("WeakenedEnd"))),
						delay(optWeakenedAnimEnd)
					)
				)
			);
		};

		return BehaviorTuple(
			root(selector( "wicked-wraith",
				weakened(),
				roam(),
				attackDanmaku(),
				attackSummon(),
				attackAoe(),
				attackRanged()
			)),
			parallel("update-location",
				every(0.5s, set(actor::Target(), actor::ClosestPlayer()) ),
				every(5.0s, ifElse(
					greaterThan( actor::CountInRange( actor::Self(), 2000.0f, actorquery::is::entityType(EntityType::Wraith) ), value(0)) ||
					greaterThan( actor::CountInRange( actor::Self(), 2000.0f, actorquery::is::entityType(EntityType::Stray) ), value(1)),
					set( varCanSummon, false ),
					set( varCanSummon, true )
				) )
			)
		);
	}
}}}