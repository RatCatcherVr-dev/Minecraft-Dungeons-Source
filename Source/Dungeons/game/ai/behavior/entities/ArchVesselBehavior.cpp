#include "Dungeons.h"
#include "ArchVesselBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/EvokerFormations.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/task/HealTask.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include <DrawDebugHelpers.h>
#include "game/util/LocationQuery.h"
#include "game/component/BeaconAttackComponent.h"
#include "game/ai/task/attack/formations/VesselFormations.h"
#include "game/Enchantments/VesselTrail.h"
#include "game/ai/action/AnimActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createArchVessel(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	enum class State {
		Start,
		Moving,
		Ranging,
		Beaming,
		Summoning,		
	};	

	
	
	static Random rnd;

	enum class Direction {
		Up,
		Left,
		Down,
		Right
	};

	static const std::array<FVector2D, 4> directionsToUnit{
		FVector2D { 0.f, -1.f },
		FVector2D { -1.f, 0.f },
		FVector2D { 0.f, 1.f },
		FVector2D { 1.f, 0.f },
	};

	static std::array<std::array<Direction, 4>, 4> directionSets { {
		{ // Up
			Direction::Left,
			Direction::Right,
			Direction::Up,
			Direction::Down
		},
		{ // Left
			Direction::Up,
			Direction::Down,
			Direction::Left,
			Direction::Right
		},
		{ // Down
			Direction::Left,
			Direction::Right,
			Direction::Down,
			Direction::Up
		},
		{ // Right
			Direction::Up,
			Direction::Down,
			Direction::Right,
			Direction::Left
		}
	} };

	// should be a shared pointer but for whatever reason it errors out at runtime
	// const auto previousDirection = sharedRef(Direction::Up); 	
	static Direction previousDirection { Direction::Up };

	const auto randomizeDirections = [] {
		auto& increments = directionSets[static_cast<int>(previousDirection)];
		
		if (rnd.nextBoolean()) {
			const auto temp = increments[0];
			increments[0] = increments[1];
			increments[1] = temp;
		}

		return
			;
	};	
	
	const auto locationProvider = [&](StateRef state) -> FLocator {
		const auto startLocation = state.params().startPos;
		const auto currentLocation = state.owner->GetActorLocation();
		
		

		const float moveAroundDistanceLimit = options.Get("MoveAroundDistanceLimit", 1500.f);
		static std::array<float, 3> offsets { 700.f, 500.f, 300.f };

		randomizeDirections();
		const auto& directions = directionSets[static_cast<int>(previousDirection)];

		for (auto direction : directions) {
			const auto& unitDirection = directionsToUnit[static_cast<int>(direction)];

			for (auto offset : offsets) {
				const auto candidate = currentLocation + FVector { unitDirection.X, unitDirection.Y, 0.f } * offset;

				if (FVector::DistSquared2D(startLocation, candidate) < FMath::Square(moveAroundDistanceLimit)) {
					if (const auto maybeGround = locationquery::findGround(state.world(), candidate)) {
						previousDirection = direction;
						return FVector{ candidate.X, candidate.Y, maybeGround.GetValue() };
					}
				}
			}			
		}

		return {};
	};	

	const auto stageIndex = sharedRef(0);
	static std::array< State, 11> stageStates{
		State::Start,
		State::Ranging,
		State::Moving,
		State::Beaming,
		State::Moving,
		State::Summoning,
		State::Moving,
		State::Ranging,
		State::Beaming,	
		State::Summoning,
		State::Moving,
	};
	const auto stage = [=](StateRef state) -> State {
		return stageStates[stageIndex(state)];
	};	

	const auto incrementStage = [=](StateRef state) {
		auto oldIndex = stageIndex(state); 
		
		auto newIndex = oldIndex + 1;
		if (newIndex >= stageStates.size()) {
			newIndex = 1; //skip first state cause it is the initial delay
		}

		UE_LOG(LogTemp, Warning, TEXT("Switching state %d %d @ %f"), oldIndex, newIndex, state.world().GetTimeSeconds());
		stageIndex(state, newIndex);
	};

	return BehaviorTuple(
		root(selector("ArchVessel",
			uninterruptible(sequence(
				predicate(equals(stage, value(State::Start))),
				alwaysTrue(dropFor(options.Get("PostStartDelay", 0.6s), incrementStage))
			)),
			uninterruptible(sequence(
				predicate(equals(stage, value(State::Moving))),
				chain(					
					behavior::moveTo(
						move::From(locationProvider),
						Relative(options.Get("MoveAroundMultiplier", 1.1f))
					),
					behavior::moveTo(
						move::From(locationProvider),
						Relative(options.Get("MoveAroundMultiplier", 1.1f))
					),
					behavior::moveTo(
						move::From(locationProvider),
						Relative(options.Get("MoveAroundMultiplier", 1.1f))
					),					
					alwaysTrue(dropFor(options.Get("PostMoveDelay", 0.75s), incrementStage)),
					common::Exec([] { UE_LOG(LogTemp, Warning, TEXT("Moving Chain ended")); })
				)
			)),	
			uninterruptible(sequence(
				predicate(equals(stage, value(State::Ranging))),
				chain(
					rangedAttackInOrientation(mob, actor::Target()),
					alwaysTrue(dropFor(options.Get("PostRangingDelay", 0.75s), incrementStage)),
					common::Exec([] { UE_LOG(LogTemp, Warning, TEXT("Ranging Chain ended")); })
				)
			)),			
			uninterruptible(sequence(
				predicate(equals(stage, value(State::Beaming))),
				onStart({ 
					anim::SetLookAtTarget(actor::Target()),
					[](StateRef state) {
					const auto beamComponent = state.owner->FindComponentByClass<UBeaconAttackComponent>();
					beamComponent->MulticastStart();
				}}),
				onStop(anim::ClearLookAtTarget()),
				playAnimation(options.Get("BeamAnimation"), true),
				alwaysTrue(dropFor(options.Get("TotalBeamLifetime", 5.5s) + options.Get("PostBeamingDelay", 0.5s), incrementStage))
			)),
			uninterruptible(sequence(
				startPredicate(equals(stage, value(State::Summoning))),				
				onStart([](StateRef state) {
					if (const auto vesselTrail = state.owner->FindComponentByClass<UVesselTrail>()) {
						vesselTrail->RemoveAll();
					}					
				}),
				onStop(incrementStage),
				make_unique<USummonFormationTask>(
					options.Get("SummonSequence"),
					options.Get("SummonDuration", 5.2s),
					options.Get("SummonStart", 1.1s),
					options.Get("SummonCooldown", 4.s),
					options.Get("DisappearDelay", 1.s),
					options.Get("AppearDelay", 3.s),
					formation::makeVesselRandom(
						options.Get("CountExtras", 3),
						options.Get("TotalTime", 2.s)
					),
					USummonFormationTask::SummonMob,
					options.Get("EmergeDuration", 0.s)
				)
			))
		)),
		parallel("update-targets",
			every(0.5s, sequence(
				set(actor::Target(), actor::ClosestEnemy(3000.f))
			))
		)
	);
}

}}}
