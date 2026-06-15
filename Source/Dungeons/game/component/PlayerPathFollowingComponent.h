#pragma once

#include "Navigation/PathFollowingComponent.h"
#include "PlayerPathFollowingComponent.generated.h"

UCLASS()
class DUNGEONS_API UPlayerPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()
	
public:
	void FollowPathSegment(float DeltaTime) override;
	bool HasReachedCurrentTarget(const FVector& CurrentLocation) const override;
	void ResumeMove(FAIRequestID RequestID = FAIRequestID::CurrentRequest) override;
	void SetMoveSegment(int32 SegmentStartIndex) override;
	void UpdatePathSegment() override;

private:
	FVector PreviousFocus = FVector::ZeroVector;

	void RotatePawnTowardsTarget();
};
