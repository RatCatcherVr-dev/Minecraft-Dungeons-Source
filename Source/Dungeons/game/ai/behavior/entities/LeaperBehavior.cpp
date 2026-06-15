#include "Dungeons.h"
#include "LeaperBehavior.h"

#include "game/ai/action/AnimActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/provider/Move.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/task/attack/MeleeAttack.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/ai/task/TurnTask.h"
#include "game/util/LocationQuery.h"

#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MobCharacterMovementComponent.h"
#include "runtime/aimodule/classes/AIController.h"
#include "EngineUtils.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>

static const bool debugLeaper = false;

namespace bt { namespace behavior { namespace entities {
		
	BehaviorTuple createLeaper(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		// States
		enum class State {
			Idle,
			Approach,
			BasicAttack,
			Strafe,
			WindUp,
			Chase,
			Pounce,
			AOE,
			Attack,
			Recover,
			Flee
		};

		// Options
		auto optIdleRadius = options.Get("idle-radius", 1800.0f);
		auto optStrafeRadius = options.Get("strafe-radius", 900.0f);
		auto optStrafeSpeed = options.Get("strafe-speed", 0.5f);
		auto optStrafeDuration = options.Get("strafe-duration", 5.0s);
		auto optStrafeMargin = options.Get("strafe-margin", 200.0f);
		auto optPounceSpeed = options.Get("pounce-speed", 7.0f);
		auto optPounceDistance = options.Get("pounce-distance", 700.0f);
		auto optPounceDuration = options.Get("pounce-duration", 1.0s);
		auto optPounceDelay = options.Get("pounce-delay", 0.2s);
		auto optChaseRadius = options.Get("chase-radius", 500.0f);
		auto optChaseSpeed = options.Get("chase-speed", 1.8f);
		auto optFleeDuration = options.Get("flee-duration", 4.0s);
		auto optAttackRadius = options.Get("attack-radius", 400.0f);
		auto optAOERange = options.Get("aoe-range", 400.0f);
		auto optWindUpDuration = options.Get("windup-duration", 1.5s);
		auto optRecoveryDuration = options.Get("recovery-duration", 5.0s);

		// Vars
		const auto mobState = sharedRef(State::Idle);
		const auto strafeClockwise = makeSharedRef<bool>(false);
		const auto positionCache = sharedRef(FVector(0, 0, 0));
		const auto AOEHit = makeSharedRef<bool>(false);
		const auto strafeDuration = makeSharedRef<bt::Duration>(optStrafeDuration);
		static Random rnd;

		// Provides the pounce target destination.
		const auto pounceLocationProvider = [=](StateRef state) -> FLocator {
			if( auto targetActor = Cast<ABaseCharacter>(actor::Target()(state)) ) {
				auto sourceActor = Cast<AMobCharacter>(state.owner);
				if( auto ai = sourceActor->AiController() ) {
					auto target = targetActor->GetActorLocation();
					auto source = sourceActor->GetActorLocation();

					auto delta = target - source;
					float distance = optPounceDistance;

					delta.Normalize();
					auto goal = source + delta * distance;

					if (const auto maybeGround = locationquery::findGround(state.world(), goal)) {
						goal.Z = maybeGround.GetValue();
					}

					return goal;
				}
			}

			return {};
		};

		// Provides the strafe target destination.
		const auto strafeLocationProvider = [=](StateRef state) -> FLocator {
			if( auto targetActor = Cast<ABaseCharacter>(actor::Target()(state)) ) {
				auto sourceActor = Cast<AMobCharacter>(state.owner);
				auto target = targetActor->GetActorLocation();
				auto source = sourceActor->GetActorLocation();

				auto delta = target-source;
				auto delta2 = delta.RotateAngleAxis( *strafeClockwise ? 90.0f : -90.0f, FVector::UpVector );

				auto target2 = source + delta2;

				auto rvec = target2 - target;
				rvec.Normalize();

				float factor = optStrafeRadius;

				{
					float val = delta.SizeSquared();
					float cha = optChaseRadius * optChaseRadius;
					float idl = optIdleRadius * optIdleRadius;
					factor = 1.0f - ( (val-cha) / (idl-cha) );
					factor = FMath::Clamp(factor, 0.0f, 1.0f);
					factor *= optStrafeRadius;
				}

				auto goal = target + (rvec * factor);

				if (const auto maybeGround = locationquery::findGround(state.world(), goal)) {
					goal.Z = maybeGround.GetValue();
				}

				if( debugLeaper) {
					DrawDebugSphere(&state.world(), goal, 100.0f, 20, FColor::Yellow, false, 0.5f, 0, 3.0f);
					DrawDebugSphere(&state.world(), positionCache, 100.0f, 20, FColor::Blue, false, 0.5f, 0, 3.0f);
				}
				return goal;
			}

			return {};
		};

		return BehaviorTuple(
			root(selector("Leaper",

				// Approach the player
				sequence( "Approach",
					predicate(equals(mobState, value(State::Approach))),
					parallel(
						playAnimation( options.Get( "Walk" ) ),
						moveTo( actor::Target(), Relative(optChaseSpeed) ),
						sequence(
							predicate( locator::IsInRange( actor::Target(), optStrafeRadius + optStrafeMargin*0.9f ) ),
							set( mobState, value(State::Strafe) )
						)
					)
				),

				sequence( "BasicAttack",
					predicate(equals(mobState, value(State::BasicAttack))),
					parallel(
						chain(
							std::make_unique<UMeleeAttack>(actor::Target(), true, 2),
							set( mobState, value(State::Strafe) )
						),
						dropFor(1.5s, set(mobState, value(State::Strafe)))
					)
				),

				// Strafe around the player.
				sequence( "Strafe",
					predicate(equals(mobState, value(State::Strafe))),
					onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
					onStop(focus::Clear(EAIFocusPriority::Attacking)),
					parallel(
						sequence("strafing-animation",
							ifElse( predicate( equals( strafeClockwise, value(true) ) ),
								playAnimation( options.Get( "StrafeR" ) ),
								playAnimation( options.Get( "StrafeL" ) )
							),
							moveTo( location::From( strafeLocationProvider ), Relative( optStrafeSpeed ), true )
						),
						sequence(
							predicate(!locator::IsInRange( actor::Target(), optIdleRadius )),
							set( mobState, value(State::Idle ) )
						),
						sequence(
							predicate(!locator::IsInRange( actor::Target(), optStrafeRadius + optStrafeMargin )),
							set( strafeDuration, minus( strafeDuration, value( 2.0s ) )),
							set( mobState, value(State::Approach ) )
						),
						sequence(
							predicate( locator::IsInRange( actor::Target(), optAttackRadius ) ),
							set( strafeDuration, minus( strafeDuration, value( 2.0s ) )),
							set( mobState, value(State::BasicAttack) )
						),
						ifElse(
							predicate(lessThan( strafeDuration, value(0.0s) )),
							sequence(
								set(strafeDuration, value(optStrafeDuration)),
								set( mobState, value(State::WindUp) )
							),
							dropFor( strafeDuration,
								sequence(
									set(strafeDuration, value(optStrafeDuration)),
									set(mobState, value(State::WindUp))
								)
							)
						)
					)
				),

				// Pause and perform a windup animation.
				sequence( "WindUp",
					predicate(equals(mobState, value(State::WindUp))),
					chain(
						playAnimation( options.Get( "WindUp" ), false, true, true ),
						delay(optWindUpDuration),
						set( mobState, value(State::Chase) )
					)
				),

				// Chase down the player until we're with in pounce range.
				sequence( "Chase",
					predicate(equals(mobState, value(State::Chase))),
					parallel(
						chain(
							moveTo( 
								move::withSettings( 
									actor::Target(), 
									move::defaultRequest().SetAcceptanceRadius( optChaseRadius )
								),
								Relative( optChaseSpeed )
							),
							set( mobState, value(State::Pounce) )
						),
						sequence(
							predicate( locator::IsInRange( actor::Target(), optChaseRadius ) ),
							set( mobState, value(State::Pounce) )
						)
					)
				),

				// Pounce towards the player.
				sequence( "Pounce",
					predicate(equals(mobState, value(State::Pounce))),
					parallel(
						playAnimation( options.Get( "Pounce" ) ),//, false, true, true ),
						chain(
							delay(optPounceDelay),
							moveTo( location::From( pounceLocationProvider ), Relative( optPounceSpeed ) )
						),
						dropFor(optPounceDuration, set( mobState, value(State::AOE)))
					)
				),

				// Instant AOE attack when the pounce has completed.
				sequence("AOE",
					predicate( equals(mobState, value(State::AOE)) ),
					chain(
						std::make_unique<UAoeAttack>(true),
						ifElse( predicate(locator::IsInRange(actor::Target(), optAOERange)),
							sequence(
								set( AOEHit, value(true) ),
								set( mobState, value(State::Recover) )
							),
							sequence(
								delay(0.1s),
								set( mobState, value(State::Strafe) )	
							)
						)
					)
				),

				// Follow up attacks.
				sequence( "Attack",
					predicate(equals(mobState, value(State::Attack))),
					chain(
						std::make_unique<UMeleeAttack>(actor::Target(), true, 0),
						std::make_unique<UMeleeAttack>(actor::Target(), true, 1),
						ifElse(predicate(equals( AOEHit, value(true) )),
							set( mobState, value(State::Recover) ),
							set( mobState, value(State::Flee) )
						),
						set( AOEHit, value(false) )
					)
				),

				// Pause and play the recovery animation.
				sequence( "Recover",
					predicate(equals(mobState, value(State::Recover))),
					parallel(
						playAnimation( options.Get("Recovery"), true ),
						dropFor( optRecoveryDuration, set( mobState, value(State::Strafe)) )
					)
				),

				// Run away from the player.
				sequence( "Flee",
					predicate(equals(mobState, value(State::Flee))),
					parallel(
						chain(
							behavior::moveTo(move::LocationAwayFrom( actor::Target(), 1400.0f, 1450.0f ), Relative(1.6f) ),
							set(mobState, value(State::Strafe))
						),
						dropFor(optFleeDuration, set(mobState, value(State::Strafe)))
					)
				),

				// Default idle behavior.
				sequence( "Idle",
					predicate(equals(mobState, value(State::Idle))),
					defaultRoam(),
					sequence(
						isInRange( actor::Target(), optIdleRadius ),
						set(mobState, value(State::Approach))
					)
				)

			)),
			parallel(
				"update-targets",

				// Retarget to the closest player.
				every( 0.5s,
					set(actor::Target(), actor::ClosestEnemy())
				),

				/* Debug
				every( 0.2s,
					common::Exec([=]{ 
						GEngine->AddOnScreenDebugMessage( -1, 0.19f, FColor::Cyan, 
							FString("Time = ") + FString::SanitizeFloat(strafeDuration->Seconds()));
					})
				),
				//*/	

				// Check if we're stuck strafing, in which case swap direction.
				every( 1.0s * rnd.nextFloat(0.5f, 1.2f),
					set( positionCache, [=](StateRef state){ 
						auto newPosition = Cast<AMobCharacter>(state.owner)->GetActorLocation(); 
						float dirDiff = 100.0f;
						if( (newPosition - positionCache).SizeSquared() < (dirDiff * dirDiff) )
						{
							*strafeClockwise = !(*strafeClockwise);
						}

						return Cast<AMobCharacter>(state.owner)->GetActorLocation(); 
					})
				)
			)
		);
	}
	
}}}
