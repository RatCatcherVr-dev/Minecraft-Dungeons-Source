#include "Dungeons.h"
#include "AncientGuardianBehavior.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/ChaseAndMeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtProbability.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/AncientGuardianComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/IsInRange.h"
#include "util/Random.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/task/attack/BeamAttack.h"
#include "game/component/BeamAttackComponent.h"
#include "game/component/AoeAttackComponent.h"
#include "game/ai/behavior/BeamAttackBehaviour.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/ai/task/attack/formations/AncientGuardianFormations.h"
#include "game/util/LocationQuery.h"
#include "game/ai/provider/ActorStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createAncientGuardian(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		static Random rnd;

		UBeamAttackComponent* beamComponent = mob.FindComponentByClass<UBeamAttackComponent>();
		UAoeAttackComponent* aoeComponent = mob.FindComponentByClass<UAoeAttackComponent>();
		UAncientGuardianComponent* ancientGuardianComponent = mob.FindComponentByClass<UAncientGuardianComponent>();

		const auto optBeamRange = beamComponent->GetAttackRange();
		const auto optAoeRange = aoeComponent->GetAttackRange();

		const auto optAoeDuration = aoeComponent->totalTime;

		const auto optMinBeamDelay = options.Get("MinBeamDelay", 10.0s);
		const auto optMinBeamDistance = options.Get("MinBeamDistance", 800.0f);
		const auto optTotalBeamLifetime = options.Get("TotalBeamLifetime", 5.0s);
		const auto optMaxChaseDistance = options.Get("MaxChaseDistance", 1000.0f);
		const auto optChaseSpeedMultiplier = options.Get("ChaseSpeedMultiplier", 2.0f);

		const auto optTimeBetweenMines = options.Get("TimeBetweenMines", 7.0s);
		const auto optMineRange = options.Get("MineRange", 1000.0f);

		const auto optSpikeRetractDuration = options.Get("SpikeRetractDuration", 10.0s);

		const auto optGuardianSpawnFrequency = options.Get("GuardianSpawnFrequency", 10.0s);
		const auto optGuardianSpawnHealthThreshold = options.Get("GuardianSpawnHealthThreshold", 0.5f);
		const auto optElderGuardianSpawnFrequency = options.Get("ElderGuardianSpawnFrequency", 10.0s);
		const auto optSummonRadius = options.Get("SummonRadius", 1000.0f);

		const auto optRetreatedBeamLifetime = options.Get("RetreatedBeamLifetime", 2.0s);
		const auto optRetreatSpeedMultiplier = options.Get("RetreatSpeedMultiplier", 2.0f);

		const auto optFirstRetreatHealthFraction = options.Get("FirstRetreatHealthFraction", 0.75f);
		const auto optSecondRetreatHealthFraction = options.Get("SecondRetreatHealthFraction", 0.5f);
		const auto optThirdRetreatHealthFraction = options.Get("ThirdRetreatHealthFraction", 0.25f);

		const auto optRamAttackRange = options.Get("RamAttackRange", 650.0f);
		const auto optMinRamDelay = options.Get("MinRamDelay", 4.0s);

		const auto bFirstRetreatDone = makeSharedRef<bool>(false);
		const auto bSecondRetreatDone = makeSharedRef<bool>(false);
		const auto bThirdRetreatDone = makeSharedRef<bool>(false);

		const auto bRetreating = makeSharedRef<bool>(false);

		auto IsState = [=](EAncientGuardianState ancientGuardianState) -> bt::Pred {
			return[=](bt::StateRef state) {
				return ancientGuardianComponent->GetAncientGuardianState() == ancientGuardianState;
			};
		};

		auto SetState = [=](EAncientGuardianState newState) -> bt::Action {
			return[=](bt::StateRef state) {
				ancientGuardianComponent->SetAncientGuardianState(newState);
			};
		};

		const auto guardianGroup = game::mobspawn::sampleWithProbabilities(
			std::vector<EntityType> {
			EntityType::Guardian,
			},
			{ 1.f }
		);

		const auto elderGuardianGroup = game::mobspawn::sampleWithProbabilities(
			std::vector<EntityType> {
			EntityType::ElderGuardian,
			},
			{ 1.f }
		);

		const auto retreatedSpawnGroup = game::mobspawn::sampleWithProbabilities(
			std::vector<EntityType> {
			EntityType::Guardian
		},
			{ 1.f }
			);

		auto SummonMob = [=, &options]() {
			return sequence("Summon",
				ifElse(predicate(lessThan(actorStats::healthFraction(), value(optGuardianSpawnHealthThreshold))),
					sequence(
						minTimeBetweenStopAndStart(optGuardianSpawnFrequency),
						sequence(
							playAnimation(options.Get("SummonMinions")),
							make_unique<USummonTask>(
								1,
								elderGuardianGroup,
								1.0s,
								optElderGuardianSpawnFrequency,
								value(1),
								locator::RandomReachablePointAround(actor::Self(), optSummonRadius)
							)
						)
					),
					sequence(
						minTimeBetweenStopAndStart(optElderGuardianSpawnFrequency),
						sequence(
							playAnimation(options.Get("SummonMinions")),
							make_unique<USummonTask>(
								4,
								guardianGroup,
								1.0s,
								optGuardianSpawnFrequency,
								value(2),
								locator::RandomReachablePointAround(actor::Self(), optSummonRadius)
							)
						)
					)					
				)
			);
		};

		auto Beam = [=]() {
			return[=](bt::StateRef state) {
				beamComponent->AttackLocal(actor::Target()(state));
			};
		};

		auto BeamStop = [=]() -> bt::Action {
			return[=](bt::StateRef state) {
				beamComponent->StopAttack();
			};
		};

		auto HasFormations = [=]() -> bt::Pred {
			return[=](bt::StateRef state) {
				return ancientGuardianComponent->mineFormations.Num() > 0;
			};
		};

		auto GetRandomFormation = [=]() -> TArray<FVector> {
			TArray<FVector> validPositions;
			if (ancientGuardianComponent->mineFormations.Num() > 0) {
				FFormationPositions chosenFormation = ancientGuardianComponent->mineFormations[rnd.nextInt(ancientGuardianComponent->mineFormations.Num())];
				for (int i = 0; i < chosenFormation.spawnPoints.Num(); ++i) {
					if (chosenFormation.spawnPoints[i] && chosenFormation.spawnPoints[i]->CanUse() && chosenFormation.positions.Num() >= i + 1) {
						validPositions.Add(chosenFormation.positions[i]);
					}
				}
			}
			return validPositions;
		};

		auto PlaceMines = [=, &options]() {
			return uninterruptibleSequence("Mine",				
				predicate(HasFormations()),
				minTimeBetweenStopAndStart(optTimeBetweenMines),
				maxRunTime(3.0s),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				make_unique<USummonFormationTask>(
					options.Get("SummonMines"),
					2.0s, 0.0s, 0.8s, 0.0s, 0.0s,
					formation::AncientGuardianMineFormation(GetRandomFormation)
				)
			);
		};

		auto SetRetreatLocation = [=]() {
			return[=](bt::StateRef state) {
				ancientGuardianComponent->SetNextReatreatLocation();
			};
		};

		auto GetRetreatLocation = [=]() -> bt::locator::Provider {
			return[=](bt::StateRef state) {
				return ancientGuardianComponent->GetNextReatreatLocation();
			};
		};

		auto MakeInvulnerable = [=]() -> bt::Action {
			return[=](bt::StateRef state) {
				auto mob = state.owner;
				mob->ApplyInvulnerability(-1.0f, false);
				mob->SetTargetable(false);
				return true;
			};
		};

		auto MakeVulnerable = [=]() -> bt::Action {
			return[=](bt::StateRef state) {
				auto mob = state.owner;
				mob->RemoveInvulnerability();
				mob->SetTargetable(true);
				return true;
			};
		};

		return BehaviorTuple(
			root(selector("Ancient Guardian",

				sequence(
					predicate(IsState(EAncientGuardianState::Disabled)),
					alwaysTrue()
				),
				uninterruptibleSequence(
					predicate(IsState(EAncientGuardianState::Retreating)),
					anim::ClearLookAtTarget(),
					focus::Clear(EAIFocusPriority::Attacking),
					parallel("Retreat",
						behavior::moveTo(move::withSettings(move::From(GetRetreatLocation()), move::defaultRequest().SetUsePathfinding(false)), Relative(optRetreatSpeedMultiplier)),
						sequence(
							isInRange(GetRetreatLocation(), 500.0f),
							SetState(EAncientGuardianState::Retreated),
							set(bRetreating, value(false))
						)
					)
				),
				sequence("Alcove Attack",
					predicate(IsState(EAncientGuardianState::Retreated)),
					onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
					onStop(focus::Clear(EAIFocusPriority::Attacking)),
					selector(
						sequence("Beam",
							minTimeBetweenStopAndStart(4.0s),
							playAnimation(options.Get("Beam")),
							onStart({
								anim::SetLookAtTarget(actor::Target()),
								Beam()
							}),
							onStop({
								anim::ClearLookAtTarget(),
								BeamStop()
							})
						),
						PlaceMines(),
						sequence(
							playAnimation(options.Get("SummonMinions")),
							make_unique<USummonTask>(
								6,
								retreatedSpawnGroup,
								1.0s,
								optGuardianSpawnFrequency,
								value(3),
								locator::RandomReachablePointAround(actor::Target(), optSummonRadius)
							)
						)
					)
				),

				sequence("Run Down",
					predicate(IsState(EAncientGuardianState::Return)),
					sequence(
						BeamStop(),
						sequence(
							parallel(
								behavior::moveTo(move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(false)), Relative(optChaseSpeedMultiplier)),
								sequence(
									isInRange(actor::Target(), optAoeRange - 50.0f),
									playAnimation(options.Get("SpikeAttack")),
									behavior::aoeAttack(mob, FName("SpikeAttack")),
									delay(optAoeDuration),
									PlaceMines(),
									onStop(SetState(EAncientGuardianState::Normal))
								)
							)
						)
					)
				),

				uninterruptibleSequence("No Spikes Flee",
					predicate(IsState(EAncientGuardianState::NotSpiking)),
					onStart({ 
						anim::ClearLookAtTarget(),
						focus::Clear(EAIFocusPriority::Attacking)
					}),
					parallel(
						behavior::defaultFleeFrom(actor::Target(), 5000.0f),
						dropFor(optSpikeRetractDuration,
							chain(
								playAnimation(options.Get("SpikeShow")),
								SetState(EAncientGuardianState::Normal)
							)
						)
					)
				),

				sequence("Attacks",
					predicate(IsState(EAncientGuardianState::Normal)),
					selector(						

						sequence("Beam",
							isInRange(actor::Target(), FloatRange(optMinBeamDistance, optBeamRange)),
							uninterruptibleSequence("Beam",
								minTimeBetweenStopAndStart(optMinBeamDelay),
								playAnimation(options.Get("Beam")),
								onStart({
									anim::SetLookAtTarget(actor::Target()),
									focus::Set(actor::Target(), EAIFocusPriority::Attacking),
									Beam()
								}),
								delay(optTotalBeamLifetime),
								onStop({
									anim::ClearLookAtTarget(),
									SetState(EAncientGuardianState::NotSpiking),
									BeamStop()
								})
							)
						),

						sequence("Mines",
							isInRange(actor::Target(), optMineRange),
							PlaceMines()
						),

						SummonMob(),

						sequence("Ram",
							isInRange(actor::Target(), optRamAttackRange),
							playAnimation(options.Get("RamAttack")),
							behavior::aoeAttack(mob, FName("RamAttack"))
						),
						
						behavior::chase(actor::Target(), FloatRange(100.0f, optMaxChaseDistance), Relative(optChaseSpeedMultiplier))
					))
			)),
			parallel("Update",
				every(0.5s, set(actor::Target(), actor::ClosestEnemy(10000.0f))),
				parallel("Retreat Check",
					sequence("First Retreat",
						predicate(equals(bFirstRetreatDone, value(false))),
						predicate(lessThanOrNearlyEquals(actorStats::healthFraction(), value(optFirstRetreatHealthFraction))),
						SetRetreatLocation(),
						set(bRetreating, value(true)),
						SetState(EAncientGuardianState::Retreating),
						set(bFirstRetreatDone, value(true))
					),
					sequence("Second Retreat",
						predicate(equals(bSecondRetreatDone, value(false))),
						predicate(lessThanOrNearlyEquals(actorStats::healthFraction(), value(optSecondRetreatHealthFraction))),
						SetRetreatLocation(),
						set(bRetreating, value(true)),
						SetState(EAncientGuardianState::Retreating),
						set(bSecondRetreatDone, value(true))
					),
					sequence("Third Retreat",
						predicate(equals(bThirdRetreatDone, value(false))),
						predicate(lessThanOrNearlyEquals(actorStats::healthFraction(), value(optThirdRetreatHealthFraction))),
						SetRetreatLocation(),
						set(bRetreating, value(true)),
						SetState(EAncientGuardianState::Retreating),
						set(bThirdRetreatDone, value(true))
					)
				)
			)
		);
	}
}}}
