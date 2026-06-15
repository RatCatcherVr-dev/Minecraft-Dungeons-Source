#include "Dungeons.h"
#include "TempestGolemBehaviour.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/debug/BtDebug.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/MeleeTicketProvider.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "util/Random.h"
#include "util/CharacterQuery.h"
#include "game/component/TempestGolemComponent.h"
#include "game/ai/task/attack/AoeAttack.h"


class UBehaviorOptionsComponent;

namespace bt { namespace behavior { namespace entities {
	int shieldTargetCounter = 0;	

	BehaviorTuple createTempestGolem(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		const auto optArmsAttackRange = options.Get("arms-attack-range", 1200.0f);
		const auto optArmsConeAngle = options.Get("arms-cone-angle", PI * 0.3f);

		const auto optShieldAttackRange = options.Get("shield-attack-range", 800.0f);
		const auto optShieldTriggerTime = options.Get("shield-trigger-time", 8.0f);
		const auto optShieldConeAngle = options.Get("shield-cone-angle", PI * 0.8f);

		const auto optRangeAttackRange = options.Get("range-attack-range", 1201.0f);

		const auto optLeftArmCooldown = options.Get("leftarm-cooldown", 5.f);
		const auto optRightArmCooldown = options.Get("rightarm-cooldown", 5.f);
		const auto optRangeAttackCooldown = options.Get("range-attack-cooldown", 10.f);
		
		const auto optDebugLeftArm = options.Get("debug-left-arm", 0);
		const auto optDebugRightArm = options.Get("debug-right-arm", 0);
		const auto optDebugShield = options.Get("debug-shield", 0);

		const auto bJustAttacked = makeSharedRef<bool>(false);
		const auto bLeftArmAttacked = makeSharedRef<bool>(false);
		const auto bRightArmAttacked = makeSharedRef<bool>(false);
		const auto bRangeAttacked = makeSharedRef<bool>(false);

		auto tempestComponent = mob.FindComponentByClass<UTempestGolemComponent>();

		auto CanArmAttack = [=, &tempestComponent] (TempestArmType armType) -> bt::Pred {
			return[=](StateRef state) {
				return tempestComponent->CanArmAttack(armType);
			};
		};
		auto ArmAttack = [=, &tempestComponent](TempestArmType armType) -> bt::Action {
			return[=](StateRef state) {
				tempestComponent->StartArmTracking(armType);
			};
		};
		auto SetArmStatus = [=, &tempestComponent](TempestArmType armType, TempestArmStatus newStatus) -> bt::Action {
			return[=](StateRef state) {
				tempestComponent->SetArmStatus(armType, newStatus);
			};
		};

		const auto ClosestPlayerPredicate = [&](const AActor* v) {
			if (auto character = Cast<APlayerCharacter>(v)) {
				return characterquery::is::hostile(character) && characterquery::is::targetable(character) && actorquery::is::alive(character);
			}
			return false;
		};

		const auto GetActorsRight = [=, &mob]() -> bt::Action {
			return [=, &mob](StateRef state) {
				if (tempestComponent->CanArmAttack(TempestArmType::RIGHT) || tempestComponent->IsArmTracking(TempestArmType::RIGHT))
				{
					FVector direction = ((mob.GetActorForwardVector()) + mob.GetActorRightVector()).GetSafeNormal2D();
					FVector OffSet = FVector(600.f, 0.f, 0) * direction;

					auto actorsWithAngle = actorquery::getActorsInCone2DWithAngle(&mob, OffSet, direction, APlayerCharacter::StaticClass(), optArmsAttackRange, optArmsConeAngle, ClosestPlayerPredicate, optDebugRightArm != 0);
					//GEngine->AddOnScreenDebugMessage(-1, 0.08f, FColor::Red, FString::Printf(TEXT("Right Arm Weight: %f"), actorsWithAngle.actorAngle));
					tempestComponent->rightArmAttackWeight = actorsWithAngle.actorAngle;
					tempestComponent->rightTargets = actorsWithAngle.actors;
				}
			};
		};
		const auto GetActorsLeft = [=, &mob]() -> bt::Action {
			return [=, &mob](StateRef state) {
				if (tempestComponent->CanArmAttack(TempestArmType::LEFT) || tempestComponent->IsArmTracking(TempestArmType::LEFT))
				{
					FVector direction = ((mob.GetActorForwardVector()) + (-mob.GetActorRightVector())).GetSafeNormal2D();

					FVector OffSet = FVector(600.f, 0.f, 0) * direction;

					auto actorsWithAngle = actorquery::getActorsInCone2DWithAngle(&mob, OffSet, direction, APlayerCharacter::StaticClass(), optArmsAttackRange, optArmsConeAngle, ClosestPlayerPredicate, optDebugLeftArm != 0);
					//GEngine->AddOnScreenDebugMessage(-1, 0.08f, FColor::Red, FString::Printf(TEXT("Left Arm Weight: %f"), actorsWithAngle.actorAngle));
					tempestComponent->leftArmAttackWeight = actorsWithAngle.actorAngle;
					tempestComponent->leftTargets = actorsWithAngle.actors;
				}
			};
		};
		const auto GetActorsShield = [=, &mob]() -> bt::Action {
			return [=, &mob](StateRef state) {
				if (tempestComponent->CanArmAttack(TempestArmType::SHIELD) || tempestComponent->IsArmTracking(TempestArmType::SHIELD))
				{
					auto actors = actorquery::getActorsInCone2DWithAngle(&mob, FVector::ZeroVector, mob.GetActorForwardVector(), APlayerCharacter::StaticClass(), optShieldAttackRange, optShieldConeAngle, ClosestPlayerPredicate, optDebugShield != 0);
					tempestComponent->shieldTargets = actors.actors;
				}			
			};
		};

		const auto SetLookAtLocation = [=, &mob]() -> bt::Action {
			return[=, &mob](StateRef state) {
				auto actors = actorquery::getActorsInCone2DWithAngle(&mob, FVector::ZeroVector, mob.GetActorForwardVector(), APlayerCharacter::StaticClass(), optShieldAttackRange, optShieldConeAngle, ClosestPlayerPredicate, false);
				if (auto closestActor = actorquery::getClosestActorFromArray(&mob, actors.actors))
				{
					tempestComponent->lookAtLocation = closestActor->GetActorLocation();
				}
				else
				{
					if (AActor* closestEnemy = actor::ClosestEnemy(20000)(state))
					{
						tempestComponent->lookAtLocation = closestEnemy->GetActorLocation();
					}
				}				
			};
		};

		auto HasTargetRight = [=]() -> bt::Pred {
			return[=](StateRef state) {
				return tempestComponent->rightTargets.Num() > 0;
			};
		};
		auto HasTargetLeft = [=]() -> bt::Pred {
			return[=](StateRef state) {
				return tempestComponent->leftTargets.Num() > 0;
			};
		};
		auto HasTargetShield = [=]() -> bt::Pred {
			return[=](StateRef state) {
				return tempestComponent->shieldTargets.Num() > 0;
			};
		};

		auto IsTempestActive = [=, &tempestComponent]() -> bt::Pred {
			return[=](StateRef state) {
				return tempestComponent->IsTempestActive();
			};
		};


		return BehaviorTuple(
			root(selector("TempestGolem",
				sequence("Main",
					predicate(IsTempestActive()),
					parallel("ArmAttack",
						sequence("RightArm",
							predicate(HasTargetRight()),
							predicate(CanArmAttack(TempestArmType::RIGHT)),
							predicate(equals(bRightArmAttacked, value(false))),
							onStart(set(bRightArmAttacked, value(true))),
							onStop(set(bRightArmAttacked, value(false))),
							minTimeBetweenStopAndStart(optRightArmCooldown),
							sequence(
								ArmAttack(TempestArmType::RIGHT)
							)
						),
						sequence("LeftArm",
							predicate(CanArmAttack(TempestArmType::LEFT)),
							predicate(HasTargetLeft()),
							predicate(equals(bLeftArmAttacked, value(false))),
							onStart(set(bLeftArmAttacked, value(true))),
							onStop(set(bLeftArmAttacked, value(false))),
							minTimeBetweenStopAndStart(optLeftArmCooldown),
							sequence(
								ArmAttack(TempestArmType::LEFT)
							)
						),
						sequence("ShieldArms",
							predicate(CanArmAttack(TempestArmType::SHIELD)),
							predicate(HasTargetShield()),
							sequence(
								ArmAttack(TempestArmType::SHIELD)
							)
						),
						sequence("RangeAttack",
							predicate(locator::IsInRange(actor::Target(), FloatRange(optRangeAttackRange, 20000))),
							/*predicate(equals(bRangeAttacked, value(false))),*/
							onStart(set(bRangeAttacked, value(true))),
							onStop(set(bRangeAttacked, value(false))),
							minTimeBetweenStopAndStart(optRangeAttackCooldown),
							sequence(
								std::make_unique<UAoeAttack>(actor::Target(), "RangeAttack"),
								delay(optRangeAttackCooldown)
							)
						),
						sequence("LookAtTarget",
							SetLookAtLocation()
						)
					)
				)
			)),			
			parallel(
				every(0.1s, GetActorsShield()),
				every(0.1s, GetActorsRight()),
				every(0.1s, GetActorsLeft()),
				every(0.1s, set(actor::Target(), actor::ClosestEnemy()))
			)
		);
	}
}}}
