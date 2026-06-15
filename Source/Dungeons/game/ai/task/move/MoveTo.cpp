#include "Dungeons.h"
#include "MoveTo.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "AIController.h"
#include "game/actor/character/mob/MobBtController.h"
#include "game/ai/provider/Move.h"
#include "world/entity/MobTags.h"
#include <NavigationSystem.h>

DECLARE_CYCLE_STAT(TEXT("STAT_UMoveTo_OnCanRun"), STAT_UMoveTo_OnCanRun, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("STAT_UMoveTo_OnCanContinue"), STAT_UMoveTo_OnCanContinue, STATGROUP_AI);

UMoveTo::UMoveTo(const bt::location::Provider& locationProvider, bool continuously, const FAIMoveRequest& _moveRequest)
	: type { Type::Location }
	, locationProvider { locationProvider }
	, continuously { continuously }
	, moveRequest(_moveRequest) {
}

UMoveTo::UMoveTo(const bt::move::Provider& moveRequestProvider)
	: type { Type::Move }
	, moveRequestProvider { moveRequestProvider }
	, continuously { false } {
}

bool UMoveTo::OnCanRun(bt::StateRef state) {
	SCOPE_CYCLE_COUNTER(STAT_UMoveTo_OnCanRun);
	if (type == Type::Move) {
		moveRequest = moveRequestProvider(state);
	} else {
		const auto location = locationProvider(state);
		moveRequest.SetGoalLocation(location);
		moveRequest.SetAllowPartialPath(true);

		//DrawDebugSphere(&state.world(), location, 16, 16, FColor::White, true, 2.f);
	}

	if (moveRequest.IsMoveToActorRequest() && !moveRequest.GetGoalActor()) {
		return false;
	}

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
	bool validQuery = state.controller->BuildPathfindingQuery(moveRequest, currentQuery);
	
#if PLATFORM_WINDOWS 
	//D11.SC removing this for consoles as its expensive and can stall the ai process by nearly half a ms (its not that big of an issue on PC and i dont want the overhead of config variable checking on consoles)
	if (moveRequest.IsUsingPathfinding())
	{
		validQuery = validQuery && FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->TestPathSync(currentQuery, EPathFindingMode::Hierarchical);
	}
#else
	if (moveRequest.IsUsingPathfinding() && hasMobTag(state.owner, MobTags::HashTag_Miniboss)) {
		validQuery = validQuery && FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->TestPathSync(currentQuery, EPathFindingMode::Hierarchical);
	}
#endif
	
	return validQuery;

}

bool UMoveTo::OnCanContinue(bt::StateRef state) {
	SCOPE_CYCLE_COUNTER(STAT_UMoveTo_OnCanContinue);

	if (state.controller->IsPathFinding())
	{
		return true;
	}	
	//bool CanRun(state.controller->GetMoveStatus() == EPathFollowingStatus::Moving);
	//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("### UMoveTo::OnCanContinue %s  : %d %d #####\n"), *state.owner->GetFullName(), (int)CanRun, (int)state.controller->GetMoveStatus());
	return state.controller->GetMoveStatus() == EPathFollowingStatus::Moving;
}



void UMoveTo::OnStart(bt::StateRef state) {
	
	state.controller->MoveTo(moveRequest);
	if (type == Type::Move) {
		state.controller->SetFocus(moveRequest.GetGoalActor(), EAIFocusPriority::Gameplay);
	}
}


void UMoveTo::OnTick(bt::StateRef state) {
	if (type == Type::Location && continuously) {
		const auto location = locationProvider(state);


		if(FVector::DistSquared(moveRequest.GetGoalLocation(), location) >= FMath::Pow(moveRequest.GetAcceptanceRadius(), 2.0f)){
			moveRequest.UpdateGoalLocation(location);
			state.controller->MoveTo(moveRequest);
		}
		
		//DrawDebugSphere(&state.world(), location, 16, 16, FColor::White, true, 2.f);
	}
}

void UMoveTo::OnStop(bt::StateRef state) {
	state.controller->StopMovement();
	
	if (type == Type::Move) {
		state.controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
