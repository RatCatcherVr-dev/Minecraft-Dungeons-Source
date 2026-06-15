#include "Dungeons.h"
#include "PiggyBankBehavior.h"
#include "game/component/MobAnimationsComponent.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/action/StateActions.h"
#include "game/ai/provider/Locations.h"
#include "NavigationPath.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/GameBP.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/util/LocationQuery.h"
#include <NavigationSystem.h>
#include "game/actor/character/mob/MobBtController.h"

bool UBtPiggyBankFlee::OnCanRun(bt::StateRef state) {
	moveRequest = GetMoveRequest(state);
	float acceptanceRadius = moveRequest.GetAcceptanceRadius();
	if (acceptanceRadius >= 0) {
		// We could get into bad state where acceptanceRadius was ALMOST reached (e.g. 90.01 ! < 90)
		// which meant we try to move, but are immediately marked as idle
		acceptanceRadius += 10;

		auto destination = moveRequest.GetDestination();
		if (destination != FAISystem::InvalidLocation && acceptanceRadius * acceptanceRadius > FVector::DistSquared(destination, state.owner->GetActorLocation())) {
			return false;
		}
	}

	return true;
}

bool UBtPiggyBankFlee::OnCanContinue(bt::StateRef state) {
	return state.controller->GetMoveStatus() == EPathFollowingStatus::Moving;
}

void UBtPiggyBankFlee::OnStart(bt::StateRef state) {
	state.movement->bOrientRotationToMovement = true;
	state.controller->MoveTo(moveRequest);
}

void UBtPiggyBankFlee::OnStop(bt::StateRef state) {
	state.controller->StopMovement();
}


UPiggyBankFlee::UPiggyBankFlee(const bt::move::Provider& provider)
	: moveRequestProvider(provider) {
}

FAIMoveRequest UPiggyBankFlee::GetMoveRequest(bt::StateRef state) {
	return moveRequestProvider(state);
}


namespace bt { namespace move {	
	Provider PiggyBankFleeFromPlayerToLocation(const actor::Provider& provider, float radius)
	{
		return [=](StateRef state)
		{
			auto currentLocation = state.owner->GetActorLocation();
			auto player = Cast<APlayerCharacter>(provider(state));

			if (player && player->IsValidLowLevel() && &state.game() != nullptr)
			{
				FVector waypointDirection;
				FVector nextobjectiveLocation;

				if (state.game().GetNextDoor(player, nextobjectiveLocation) && FVector::Dist(currentLocation, nextobjectiveLocation) > 1000.0f) {
					waypointDirection = (nextobjectiveLocation - currentLocation);
				}
				else if (state.game().GetPreviousDoor(player, nextobjectiveLocation) && FVector::Dist(currentLocation, nextobjectiveLocation) > 1000.0f) {
					waypointDirection = (nextobjectiveLocation - currentLocation);
				}
				else {
					waypointDirection = FVector::ForwardVector;
				}

				waypointDirection.Z = 0.0f;
				waypointDirection.Normalize();

				auto checkNavigableSolid = [&state](const FVector inputLocation, FVector& outLocation)
				{
					TOptional<float> traceZ = locationquery::findGround(state.world(), inputLocation);
					if (traceZ.IsSet())
					{
						if (locationquery::isLocationOnSolidBlock(state.world(), FVector(inputLocation.X, inputLocation.Y, traceZ.GetValue())))
						{
							outLocation = FVector(inputLocation.X, inputLocation.Y, traceZ.GetValue());
							return true;
						}
					}
					return false;
				};

				auto searchInRadius = [&state, checkNavigableSolid](const FVector actorLocation, const FVector searchFirstDirection, const float searchRadius)
				{
					FVector currentSearchDirection = searchFirstDirection;

					auto tries{ 0 };
					const auto triesMax{ 16 };
					while (tries < triesMax)
					{
						if (tries > 0)
						{
							FRotator rot = searchFirstDirection.ToOrientationRotator();
							rot.Yaw += 1.0f / triesMax * tries * 360.0f;
							rot.Normalize();
							currentSearchDirection = rot.Vector();
						}

						auto searchcenter = actorLocation + currentSearchDirection*searchRadius;
						TOptional<float> traceZ = locationquery::findGround(state.world(), searchcenter);
						if (traceZ.IsSet())
						{
							searchcenter.Z = traceZ.GetValue();
							FNavLocation dest;
							FVector checkedLocation;
							if (state.movement->FindNavFloor(searchcenter, dest))
							{
								if (checkNavigableSolid(dest.Location, checkedLocation))
								{
									UNavigationSystemV1& NavSys = *UNavigationSystemV1::GetCurrent(&state.world());
									UNavigationPath* path = NavSys.FindPathToLocationSynchronously(&(state.world()), state.owner->GetActorLocation(), dest.Location, state.owner);
									if (path != nullptr && path->IsValid())
									{
										FAIMoveRequest request = defaultRequest();
										request.SetGoalLocation(checkedLocation);

										FPathFindingQuery query;
										bool validQuery = state.controller->BuildPathfindingQuery(request, query);
										if (validQuery && FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->TestPathSync(query, EPathFindingMode::Hierarchical))
										{
											return request;
										}
									}
								}
							}
						}
						tries++;
					}
					return FAIMoveRequest();
				};

				FAIMoveRequest moveRequest = searchInRadius(currentLocation, waypointDirection, radius);
				if (!moveRequest.IsValid())
				{
					moveRequest = searchInRadius(currentLocation, waypointDirection, radius*0.75f);
				}
				if (!moveRequest.IsValid())
				{
					moveRequest = searchInRadius(currentLocation, waypointDirection, radius*0.5f);
				}

				UE_CLOG(!moveRequest.IsValid(), LogDungeons, Warning, TEXT("Failed to find flee route for PiggyBank %s at location %s"), *state.owner->GetName(), *state.owner->GetActorLocation().ToString());
				return moveRequest;
			}

			return FAIMoveRequest();
		};
	}
}}


namespace bt { namespace behavior { namespace entities {

BehaviorTuple createPiggyBank(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	UMobAnimationsComponent& mobAnimationsComponent = *mob.FindComponentByClass<UMobAnimationsComponent>();

	return BehaviorTuple(
	selector("PiggyBank",
		sequence("flee-from-player",
			minTimeBetweenStopAndStart(options.Get("MaxTimeStopStart", 3.s)),
			startCondition(
				locator::IsInRange(actor::ClosestPlayer(), 800) ||
				actor::IsRecentlyDamagedFromAttack(10s)
			),
			onStart(anim::PlayBodySequence(mobAnimationsComponent.Common.Alerted)),
			parallel(
				sequence(alwaysTrue(), maxRunTime(options.Get("SomeDelay", .1s))),
				dropFor(
					options.Get("SomeDelay", .1s),
					sequence(
						onStart(speed::Set(speed::Speed(EMovementState::Running))),
						make_unique<UPiggyBankFlee>(
							move::PiggyBankFleeFromPlayerToLocation(
								actor::ClosestPlayer(),
								options.Get("FleeAwayRadius", 5800.f)
							)
						)
					)
				)
			),
			onStop(set(location::Anchor(), location::Self())),
			maxRunTime(options.Get("MaxTimeTotal", 10.s))
		),
		sequence("roam",
			minTimeBetweenStopAndStart(3.0s),
			onStart(focus::Clear()),
			behavior::moveTo(move::RandomLocationAround(location::Anchor(), 360), speed::Speed(EMovementState::Walking)) 
		)
	));
}

}}}
