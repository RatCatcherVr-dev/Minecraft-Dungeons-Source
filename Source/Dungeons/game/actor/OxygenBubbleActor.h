#pragma once

#include "Dungeons.h"
#include "CoreMinimal.h"
#include "character/player/PlayerCharacter.h"
#include "OxygenBubbleActor.generated.h"

UCLASS()
class DUNGEONS_API AOxygenBubbleActor : public AActor {
	GENERATED_BODY()
public:

	AOxygenBubbleActor();

	void Tick(float DeltaSecs) override;
	void BeginPlay() override;

	void LaunchBubble(FVector pSource, APlayerCharacter* pTarget);

	void Disable(bool AtPlayer = false);

	/** Called when the Actor is outside the hard limit on world bounds, we dont want this as they are cached */
	virtual void OutsideWorldBounds() {};

	/** Called when the actor falls out of the world 'safely' (below KillZ and such), we dont want this as they are cached  */
	virtual void FellOutOfWorld(const class UDamageType& dmgType) {};

private:
	TWeakObjectPtr<APlayerCharacter> mTarget;

	TWeakObjectPtr<UParticleSystemComponent> mParticles;

	FVector mGoalDirection;
	FVector mVelocity;

	float mTurnAroundTimeInv = 0.0f;
	float mMaxSpeed = 0.0f;
	const float mMinRandSpeed = 7500.0f;
	const float mMaxRandSpeed = 10000.0f;

	float mMinGoalCollectionRadiusSquared = 10000.0f;
	float mFlightTime = 0.0f;
	float mWaitTime = 0.0f;
};