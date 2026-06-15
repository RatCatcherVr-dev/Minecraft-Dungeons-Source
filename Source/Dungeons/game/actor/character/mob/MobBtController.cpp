#include "Dungeons.h"
#include "MobBtController.h"
#include "game/ai/behavior/BehaviorFactory.h"
#include "game/component/HealthComponent.h"
#include "game/component/BehaviorComponent.h"
#include <NavigationSystem.h>
#include "MobCharacter.h"


void AMobBtController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	if (auto healthComponent = GetPawn()->FindComponentByClass<UHealthComponent>()) {
		healthComponent->OnDeath.AddUObject(this, &AMobBtController::OnDeath);
	}

	if (auto behavior = bt::behavior::createBehaviorFor(*Mob())) 
	{
		behavior.moveTo(Mob()->Behavior);

		Mob()->Behavior->CreateCurrentBehaviourEvalState(*Mob(), 0);
		Mob()->Behavior->InitialiseBehaviourNodes();
	}
}

void AMobBtController::StopMovement()
{
	if (IsInGameThread())
	{
		Super::StopMovement();
	}
	else
	{
		//Push the stop attack to the task graph to be executed from the game thread later
		TWeakObjectPtr<AMobBtController> WeakThisComponent = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisComponent]()
		{
			if (WeakThisComponent.IsValid())
			{
				WeakThisComponent->StopMovement();
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
	}

	if (mCurrentAsyncPathQueryID != INVALID_NAVQUERYID)
	{
		//we are in the middle of an async path find, cancel it since we have been asked to move again :(
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSys)
		{
			NavSys->AbortAsyncFindPathRequest(mCurrentAsyncPathQueryID);
			mCurrentPath = nullptr;
			mCurrentAsyncPathQueryID = INVALID_NAVQUERYID;
		}
	}

}
DECLARE_CYCLE_STAT(TEXT("AMobBtController::MoveTo"), STAT_AMobBtController_MoveTo, STATGROUP_AI);

FPathFollowingRequestResult AMobBtController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath /*= nullptr*/)
{
	//D11.SC Most of this is identical to the base class, except this is Async instead

	SCOPE_CYCLE_COUNTER(STAT_AMobBtController_MoveTo);
	//UE_VLOG(this, LogAINavigation, Log, TEXT("MoveTo: %s"), *MoveRequest.ToString());

	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	if (mCurrentAsyncPathQueryID != INVALID_NAVQUERYID)
	{
		//we are in the middle of an async path find, cancel it since we have been asked to move again :(
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSys)
		{
			NavSys->AbortAsyncFindPathRequest(mCurrentAsyncPathQueryID);
			mCurrentPath = nullptr;
		}	
	}

	mCurrentAsyncPathQueryID = INVALID_NAVQUERYID;

	if (MoveRequest.IsValid() == false)
	{
		UE_VLOG(this, LogTemp, Error, TEXT("MoveTo request failed due MoveRequest not being valid. Most probably desireg Goal Actor not longer exists"), *MoveRequest.ToString());
		return ResultData;
	}

	if (GetPathFollowingComponent() == nullptr)
	{
		UE_VLOG(this, LogTemp, Error, TEXT("MoveTo request failed due missing PathFollowingComponent"));
		return ResultData;
	}

	ensure(MoveRequest.GetNavigationFilter() || !DefaultNavigationFilterClass);

	bool bCanRequestMove = true;
	bool bAlreadyAtGoal = false;
	
	if (!MoveRequest.IsMoveToActorRequest())
	{
		if (MoveRequest.GetGoalLocation().ContainsNaN() || FAISystem::IsValidLocation(MoveRequest.GetGoalLocation()) == false)
		{
			UE_VLOG(this, LogTemp, Error, TEXT("AAIController::MoveTo: Destination is not valid! Goal(%s)"), TEXT_AI_LOCATION(MoveRequest.GetGoalLocation()));
			bCanRequestMove = false;
		}

		// fail if projection to navigation is required but it failed
		if (bCanRequestMove && MoveRequest.IsProjectingGoal())
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			FNavLocation ProjectedLocation;

			if (NavSys && !NavSys->ProjectPointToNavigation(MoveRequest.GetGoalLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))
			{
				UE_VLOG_LOCATION(this, LogTemp, Error, MoveRequest.GetGoalLocation(), 30.f, FColor::Red, TEXT("AAIController::MoveTo failed to project destination location to navmesh"));
				bCanRequestMove = false;
			}

			MoveRequest.UpdateGoalLocation(ProjectedLocation.Location);
		}

		bAlreadyAtGoal = bCanRequestMove && GetPathFollowingComponent()->HasReached(MoveRequest);
	}
	else
	{
		bAlreadyAtGoal = bCanRequestMove && GetPathFollowingComponent()->HasReached(MoveRequest);
	}

	if (bAlreadyAtGoal)
	{
		UE_VLOG(this, LogTemp, Log, TEXT("MoveTo: already at goal!"));
		ResultData.MoveId = GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else if (bCanRequestMove)
	{
		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, mCurrentPFQuery);

		if (bValidQuery)
		{

			//FindPathForMoveRequest(MoveRequest, mCurrentPFQuery, mCurrentPath);


			/**
	 *	Asynchronously looks for a path from @StartLocation to @EndLocation for agent with properties @AgentProperties. NavData actor appropriate for specified
	 *	FNavAgentProperties will be found automatically
	 *	@param ResultDelegate delegate that will be called once query has been processed and finished. Will be called even if query fails - in such case see comments for delegate's params
	 *	@param NavData optional navigation data that will be used instead of the one that would be deducted from AgentProperties
	 *	@param PathToFill if points to an actual navigation path instance than this instance will be filled with resulting path. Otherwise a new instance will be created and
	 *		used in call to ResultDelegate
	 *  @param Mode switch between normal and hierarchical path finding algorithms
	 *	@return request ID
	 */
			//uint32 FindPathAsync(const FNavAgentProperties& AgentProperties, FPathFindingQuery Query, const FNavPathQueryDelegate& ResultDelegate, EPathFindingMode::Type Mode = EPathFindingMode::Regular);


			mCurrentPath = nullptr;
			mCurrentMoveRequest = MoveRequest;

			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (NavSys)
			{
				mCurrentAsyncPathQueryID = NavSys->FindPathAsync(mCurrentPFQuery.NavAgentProperties, mCurrentPFQuery, FNavPathQueryDelegate::CreateUObject(this, &AMobBtController::AsyncPathDone));
				ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

				/*if (PathResult.Result != ENavigationQueryResult::Error)
				{
					if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
					{
						if (MoveRequest.IsMoveToActorRequest())
						{
							PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
						}

						PathResult.Path->EnableRecalculationOnInvalidation(true);
						OutPath = PathResult.Path;
					}
				}
				else
				{
					UE_VLOG(this, LogAINavigation, Error, TEXT("Trying to find path to %s resulted in Error")
						, MoveRequest.IsMoveToActorRequest() ? *GetNameSafe(MoveRequest.GetGoalActor()) : *MoveRequest.GetGoalLocation().ToString());
					UE_VLOG_SEGMENT(this, LogAINavigation, Error, GetPawn() ? GetPawn()->GetActorLocation() : FAISystem::InvalidLocation
						, MoveRequest.GetGoalLocation(), FColor::Red, TEXT("Failed move to %s"), *GetNameSafe(MoveRequest.GetGoalActor()));
				}*/
			}
			
			
			
			
			//const FAIRequestID RequestID = Path.IsValid() ? RequestMove(MoveRequest, Path) : FAIRequestID::InvalidRequest;
			//if (RequestID.IsValid())
			//{
			//	bAllowStrafe = MoveRequest.CanStrafe();
			//	ResultData.MoveId = RequestID;
			//	ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;
			//
			//	if (OutPath)
			//	{
			//		*OutPath = Path;
			//	}
			//}



		}
	}

	if (ResultData.Code == EPathFollowingRequestResult::Failed)
	{
		ResultData.MoveId = GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	}

	return ResultData;
}

bool AMobBtController::IsPathFinding()
{
	if (mCurrentAsyncPathQueryID != INVALID_NAVQUERYID)
	{
		return true;
	}
	return false;
}

void AMobBtController::OnDeath() {
	if (auto mob = Mob()) {
		mob->Behavior->Clear();
	}
	UnPossess();
	Destroy();
}

AMobCharacter* AMobBtController::Mob() const {
	return static_cast<AMobCharacter*>(GetCharacter());
}

void AMobBtController::AsyncPathDone(uint32 ID, ENavigationQueryResult::Type result, FNavPathSharedPtr PathPtr)
{
	if (result != ENavigationQueryResult::Error)
	{
		if (result == ENavigationQueryResult::Success && PathPtr.IsValid())
		{
			if (mCurrentMoveRequest.IsMoveToActorRequest())
			{
				PathPtr->SetGoalActorObservation(*mCurrentMoveRequest.GetGoalActor(), 100.0f);
			}

			PathPtr->EnableRecalculationOnInvalidation(true);
			mCurrentPath = PathPtr;
		}
		
		const FAIRequestID RequestID = PathPtr.IsValid() ? RequestMove(mCurrentMoveRequest, mCurrentPath) : FAIRequestID::InvalidRequest;
		if (RequestID.IsValid())
		{
			bAllowStrafe = mCurrentMoveRequest.CanStrafe();
		}

		mCurrentAsyncPathQueryID = INVALID_NAVQUERYID;
	}
	else
	{
		UE_VLOG(this, LogTemp, Error, TEXT("Trying to find path to %s resulted in Error")
			, mCurrentMoveRequest.IsMoveToActorRequest() ? *GetNameSafe(mCurrentMoveRequest.GetGoalActor()) : *mCurrentMoveRequest.GetGoalLocation().ToString());
		UE_VLOG_SEGMENT(this, LogTemp, Error, GetPawn() ? GetPawn()->GetActorLocation() : FAISystem::InvalidLocation
			, mCurrentMoveRequest.GetGoalLocation(), FColor::Red, TEXT("Failed move to %s"), *GetNameSafe(mCurrentMoveRequest.GetGoalActor()));
	}
}
