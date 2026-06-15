#include "Dungeons.h"
#include "IllusionerBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtInvert.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/HealthActions.h"
#include "game/ai/task/attack/SummonFormationTask.h"

#include "game/ai/task/PlayAnimation.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/action/StateActions.h"
#include "game/component/RangedAttackComponent.h"

#include "game/Enchantments/Blind.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include <NavigationSystem.h>
#include "game/ai/action/AttackActions.h"

namespace bt { namespace behavior { namespace entities {

	bool isIllusioner(AActor * candidate) {
		if (auto mob = Cast<AMobCharacter>(candidate)) {
			return mob->EntityType == EntityType::Illusioner && mob->IsAlive(); 
		}
		return false;
	}

	const auto computePosition = [](float angle, float distance) {
		return FVector(
			cos(FMath::DegreesToRadians(angle)) * distance,
			sin(FMath::DegreesToRadians(angle)) * distance,
			0.0f
		);
	};

	const std::array<FVector,12> allPossibleTargets = {
		computePosition(0.0f, 600.0f),
		computePosition(60.0f, 600.0f),
		computePosition(120.0f, 600.0f),
		computePosition(180.0f, 600.0f),
		computePosition(240.0f, 600.0f),
		computePosition(300.0f, 600.0f),
		computePosition(30.0f, 800.0f),
		computePosition(90.0f, 800.0f),
		computePosition(150.0f, 800.0f),
		computePosition(210.0f, 800.0f),
		computePosition(270.0f, 800.0f),
		computePosition(330.0f, 800.0f),
	};

	BehaviorTuple createIllusioner(AMobCharacter& mob, const UBehaviorOptionsComponent& options, bool clone) {
		static Random rnd;
		URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();

		if (clone) {
			float dsq = 0.0f;
			if (auto closestMob = Cast<AMobCharacter>(actorquery::getClosestMob(&mob, 2000.0f, dsq, isIllusioner))) {
				mob.ChangeMaster(closestMob);
			}
		}

		// Options
		const auto optFleeDistance = options.Get("flee-distance", 200.0f);
		const auto optApproachDistance = options.Get("approach-distance", 1500.0f);
		const auto optApproachSpeed = options.Get("approach-speed", 1.2f);

		const auto optTeleportOutDuration = options.Get("teleport-out-duration", 1.625s);
		const auto optTeleportHiddenDuration = options.Get("teleport-hidden-duration", 1.0s);
		const auto optTeleportInDuration = options.Get("teleport-in-duration", 1.625s);
		const auto optTeleportCloneDuration = options.Get("teleport-clone-variation", 0.2f);
		const auto optTeleportCloneLifespan = options.Get("teleport-clone-lifespan", 8.0s);
		const auto optTeleportCloneInDuration = options.Get("teleport-clone-in-duration", 2.0s);
		const auto optTeleportCloneSpawnBuffer = options.Get("teleport-clone-spawn-buffer", 1.0s);
		const auto optTeleportCloneCount = static_cast<int>( options.Get( "teleport-clone-count", 5.0f ) );

		const auto optRangedMinDistance = options.Get("ranged-min-distance", 500.0f);
		const auto optRangedTargetAcquire = options.Get("ranged-target-acquire", 1000.0f);
		const auto optRangedTargetLose = options.Get("ranged-target-lose", 1200.0f);
		const auto optRangedMaxIssuedAttacks = options.Get("ranged-max-issued-attacks", 3.f);

		const auto optBlindDelay = options.Get("blind-delay", 1.0s);
		const auto optBlindCastTime = options.Get("blind-cast-time", 2.0s);
		const auto optBlindCooldown = options.Get("blind-cooldown", 12.0s);
		const auto optBlindEffect = options.GetEffect("blind-effect");
		const auto optBlindEffectDuration = options.Get("blind-effect-duration", 8.0f);
		const auto optBlindEffectHitChance = options.Get("blind-effect-hit-chance", 0.25f);
		const auto optBlindEffectRadius = options.Get("blind-effect-hit-radius", 1000.0f);

		const auto optPostTeleportDelay = options.Get("post-teleport-delay", 0.1s);
		const auto optPreTeleportDelay = options.Get("pre-teleport-delay", 0.1s);
		// Variables
		const auto vDestinations = makeSharedRef<TArray<FVector>>();
		const auto bSpawning = makeSharedRef<bool>(true);
		const auto bCloneSummonComplete = makeSharedRef<bool>(false);
		const auto bCloneTeleportComplete = makeSharedRef<bool>(false);
		const auto bCanBlind = makeSharedRef<bool>(true);


		// Lambdas
		auto getRandomDelay = [=]() -> Duration{
			return Seconds(rnd.nextFloat(optTeleportCloneDuration));
		};

		auto isClone = [clone]() -> Pred {return [clone](StateRef state) {
			return clone;
		}; };

		auto hasClone = [&]() -> Pred { return [&](StateRef state) {
			float dsq = 0.0f;
			return actorquery::getClosestMob(&mob, 3000.0f, dsq, [&](AActor* candidate) {
				if (auto mob = Cast<AMobCharacter>(candidate)) {
					return mob->EntityType == EntityType::IllusionerClone && mob->IsAlive(); 
				}
				return false;
			}) != nullptr;
		}; };

		auto canTeleport = [=]() -> Pred { return [=](StateRef state) {
			return vDestinations->IsValidIndex( 0 );
		}; };

		auto teleportDuration = [=](StateRef state) {
			return optTeleportHiddenDuration + getRandomDelay();
		};

		auto teleportDestination = [=] ( StateRef state ) -> FLocator {
			return vDestinations->Num() > 0 ? ( *vDestinations )[0] : FLocator{};
		};

		auto computeCloneDestinations = [=]() -> Action{ return [=](StateRef state) {
			vDestinations->SetNum(0, false);

			auto locationLike = state.params().target.GetLocationLike();
			if (!locationLike.IsSet()) {
				return;
			}

			auto focalPoint = locationLike.GetValue();

			auto possibleTargets = allPossibleTargets;
			std::random_shuffle(possibleTargets.begin(), possibleTargets.end());

			int targetCount = optTeleportCloneCount;
			targetCount -= actorquery::getActors<AMobCharacter>(&state.world(), [&](AActor* candidate) {
				if( auto mob = Cast<AMobCharacter>(candidate) ) {
					return mob->EntityType == EntityType::IllusionerClone && mob->IsAlive();
				}
				return false;
			}).Num();

			if( targetCount >= 0 ) {
				for( auto& possibleTarget : possibleTargets ) {
					FNavLocation out;
					auto navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world());
					if (navSystem->ProjectPointToNavigation(focalPoint + possibleTarget, out, FVector{ 10, 10, 125 })) {
						vDestinations->Emplace( out.Location );
						if (vDestinations->Num() >= targetCount) {
							break;
						}
					}
				}
			}

		}; };

		auto cloneFormation = [=](StateRef state, USummonFormationTask::AddEntryRef addEntry) {
			for( int i = 1; i < vDestinations->Num(); ++i ) {
				auto& vec = ( *vDestinations )[i];
				addEntry(getRandomDelay(), EntityType::IllusionerClone, nullptr, FTransform(vec), true, false);
			}
		};

		auto triggerCloneVFX = [=](AActor* actor) {
			auto mob = Cast<AMobCharacter>(actor);
			if (mob) {
				auto abilitySystem = mob->GetAbilitySystemComponent();
				auto vfx = [=](AActor* actor, const TOptional<FVector>& target) {
					if (target.IsSet()) {
						FGameplayCueParameters params;
						params.Instigator = mob;
						params.Location = target.GetValue();
						abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.IllusionerClone"), params);
					}
				};
				for( auto& dest : *vDestinations ) {
					vfx( actor, dest );
				}
			}
		};

		// Teleports the Illusioner to a random location and simultaneously summons
		// a few Illusioner Clone mobs .
		auto castClone = [=]() { return uninterruptibleSequence("spell-clone",
			startPredicate(!isClone()),
			startPredicate(!hasClone() || actor::IsRecentlyDamagedFromAttack(3s)),
			onStart(computeCloneDestinations()),
			onStart(set(bCloneSummonComplete, value(false))),
			onStart(set(bCloneTeleportComplete, value(false))),
			onStop(set(bCloneSummonComplete, value(false))),
			onStop(set(bCloneTeleportComplete, value(false))),
			chain(
				delay(optPreTeleportDelay),
				sequence(
					predicate(
						equals(bCloneSummonComplete, value(false)) ||
						equals(bCloneTeleportComplete, value(false))
					),
					predicate(
						canTeleport()
					),
					onStart(common::Apply(actor::Self(), triggerCloneVFX)),
					chain(
						make_unique<USummonFormationTask>(nullptr,
							0.0s, 0.0s, 0.0s, 0.0s, 0.0s,
							cloneFormation,
							USummonFormationTask::SummonMobUnderling,
							optTeleportInDuration + optTeleportHiddenDuration + optTeleportCloneSpawnBuffer
						),
						alwaysTrue( set( bCloneSummonComplete, value( true ) ) )
					),
					chain(
						make_unique<UTeleportTask>(
							optTeleportOutDuration,
							optTeleportInDuration,
							teleportDuration,
							teleportDestination,
							actor::Target()
						),
						alwaysTrue( set( bCloneTeleportComplete, value( true ) ) )
					)
				),
				delay(optPostTeleportDelay) 
			)
		); };

		// Handles the conditions for destroying a clone, such as exceeding max
		// lifetime and the clone's master Illusioner being killed.
		auto destroyClone = [=]() { return sequence("destroy-clone",
			predicate( isClone() ),
			parallel(
				sequence( 
					predicate( !actor::IsAlive(actor::Master()) ),
					common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
				),
				sequence(
					dropFor( optTeleportCloneLifespan, common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); }) )
				)
			)
		); };

		// Blinds all players in range.
		auto applyBlind = [=, &mob]() -> Action{
			return [=, &mob](StateRef state) {
				TArray< APlayerCharacter* > players;
				UActorQuery::GetPlayerCharactersInRange( &state.world(),
					state.owner->GetActorLocation(), optBlindEffectRadius, players, true
				);
				for( auto player : players ) {
					if( auto abilitySystem = mob.GetAbilitySystemComponent() ) {
						FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass( abilitySystem, optBlindEffect );
						spec.SetSetByCallerMagnitude( FName( "Duration" ), optBlindEffectDuration );
						spec.SetSetByCallerMagnitude( FName( "MeleeHitChance" ), 1.f / optBlindEffectHitChance );
						spec.SetSetByCallerMagnitude( FName( "RangedHitChance" ), 1.f / optBlindEffectHitChance );
						abilitySystem->ApplyGameplayEffectSpecToTarget( spec, player->GetAbilitySystemComponent() );
					}
				}
			};
		};

		auto castBlind = [=, blindAnim=options.Get("CastBlind")]() { 
			return uninterruptibleSequence("spell-blind",
				startPredicate( !isClone() ),
				onStart({ set(bCanBlind, value(true)), attack::OnBeforeCast() }),
				sequence(
					chain(
						playAnimation( blindAnim ),
						alwaysTrue()
					),
					chain(
						delay( optBlindDelay ),
						sequence(
							predicate( equals( bCanBlind, value(true) ) ),
							applyBlind(),
							attack::OnAfterCast(),
							set(bCanBlind, value( false ))
						),
						delay(99999s)
					),
					delay(optBlindCastTime)
				),
				minTimeBetweenStopAndStart( optBlindCooldown )
			); 
		};

		// Behavior
		return BehaviorTuple(
			root( selector( "illusioner",
				sequence("spawning",
					predicate(equals( bSpawning, value( true ) ) ),
					onStart(computeCloneDestinations()),
					parallel(
						chain( playAnimation( options.Get("Spawn") ), alwaysTrue() ),
						alwaysTrue(make_unique<USummonFormationTask>(nullptr,
							0.0s, 0.0s, 100.0s, 0.0s, 0.0s,
							cloneFormation,
							USummonFormationTask::SummonMobUnderling,
							0.0s//optTeleportInDuration + optTeleportHiddenDuration + optTeleportCloneSpawnBuffer
						)),
						alwaysTrue(dropFor( optTeleportCloneInDuration, set( bSpawning, value(false) ) ) )
					)
				),
				sequence( "passive",
					predicate( !locator::IsInRange( actor::Target(), optApproachDistance ) ),
					defaultRoam()
				),
				sequence(
					selector("aggressive",
					         chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), optApproachDistance), Relative(optApproachSpeed)),
						castBlind(),
						destroyClone(),
						castClone(),
						defaultFleeFrom(actor::Target(), optFleeDistance),
						uninterruptible(rangedAttack( mob, optRangedMinDistance, optRangedTargetAcquire, optRangedTargetLose, actor::Target(), optRangedMaxIssuedAttacks))
					)
				)
			)),
			parallel(
				every(0.5s, set(actor::Target(), actor::ClosestEnemy()) )
			)
		);
	}
} } }
