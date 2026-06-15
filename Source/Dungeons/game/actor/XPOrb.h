#pragma once

#include "GameFramework/Actor.h"
#include "XPOrb.generated.h"

class APlayerCharacter;

UCLASS()
class DUNGEONS_API AXPOrb : public AActor
{	
	GENERATED_BODY()
public:

	AXPOrb();

	void Tick(float DeltaSecs) override;
	void BeginPlay() override;

	void LaunchOrb(AActor* pSource, APlayerCharacter* pTarget);

	void DisableOrb(bool AtPlayer = false);

	void DoOrbCollect();

	/** Called when the Actor is outside the hard limit on world bounds, we dont want this as they are cached */
	virtual void OutsideWorldBounds() {};

	/** Called when the actor falls out of the world 'safely' (below KillZ and such), we dont want this as they are cached  */
	virtual void FellOutOfWorld(const class UDamageType& dmgType) {};

private:

	TWeakObjectPtr<APlayerCharacter> mTarget;

	TWeakObjectPtr<UParticleSystemComponent> mParticles;
	
	UPROPERTY()
	class UParticleSystem*  HitParticleSystemTemplate;

	FVector mGoalDirection;
	FVector mVelocity;

	float mTurnAroundTimeInv			= 0.0f;
	float mMaxSpeed					= 0.0f;
	float mMinGoalCollectionRadiusSquared	= 10000.0f;
	float mFlightTime				= 0.0f;
};
