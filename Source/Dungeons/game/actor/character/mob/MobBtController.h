#pragma once

#include "AIController.h"
#include "GameplayTagContainer.h"
#include "game/component/HealthComponent.h"
#include "MobBtController.generated.h"

UCLASS()
class DUNGEONS_API AMobBtController : public AAIController {
	GENERATED_BODY()
public:
	void OnPossess(APawn* InPawn) override;	

	/** Aborts the move the controller is currently performing */
	void StopMovement() override;

	/** Makes AI go toward specified destination
	 *  @param MoveRequest - details about move
	 *  @param OutPath - optional output param, filled in with assigned path
	 *  @return struct holding MoveId and enum code
	 */
	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr);

	bool IsPathFinding();

private:
	UFUNCTION()
	void OnDeath();

	class AMobCharacter* Mob() const;

	void AsyncPathDone(uint32 ID, ENavigationQueryResult::Type result, FNavPathSharedPtr PatPtr);

	FAIMoveRequest			mCurrentMoveRequest;
	FPathFindingQuery		mCurrentPFQuery;
	FNavPathSharedPtr		mCurrentPath;
	uint32					mCurrentAsyncPathQueryID = INVALID_NAVQUERYID;
};
