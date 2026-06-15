#pragma once

#include "CoreMinimal.h"
#include "game/actor/item/StorableItem.h"
#include "WalkPickupComponent.generated.h"

UENUM(BlueprintType)
enum class EPullInMode : uint8 {
	AccelerateWithTime,
	AccelerateWithDistance
	
};

class APlayerCharacter;

typedef std::unordered_set<APlayerCharacter*> Targets;

namespace walkPickup 
{
	enum State
	{
		Idle,
		Dropping,
		Waiting, // to move
		Moving,
		Count
	};
}

class APlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UWalkPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWalkPickupComponent();
	
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float CollisionDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float PullInThreshold = 1300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float EnablePullDelay = 0.75f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool PullInItem = true; 

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool AllowPickupWhenDead = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta = (EditCondition = "PullInItem")) 
	bool UseForceCalculation = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta = (EditCondition = "PullInItem"))
	float PullInSpeed = 250.0;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta = (EditCondition = "PullInItem", ClampMin = "0.0", ClampMax = "3.0"))
	float PullAcceleration = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta = (EditCondition = "PullInItem"))
	EPullInMode PullInMode = EPullInMode::AccelerateWithTime;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void OnRep_ServerComponentSpeed(float old);

	UFUNCTION()
	void OnRep_ServerComponentVelocity(FVector old);

	UFUNCTION(NetMulticast, Reliable)
	void NetSetPhysicsEnabled(bool enabled);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", meta = (DisplayName = "Reset Pickup values"))
	void ResetPickup();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type type) override;
	
	float GetSpeed(float dt);

private:
	bool IsAllowedToPickUp(APlayerCharacter* Player) const;	
	
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ServerComponentVelocity)
	FVector ServerComponentVelocity;

	UPROPERTY(Replicated)
	float ClosestTargetDistanceSq;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ServerComponentSpeed)
	float ServerComponentSpeed;

	float m_Acceleration;

	class USimpleMovementComponent* SimpleMovementComponent;
	FVector LastClientVelocity = FVector::ZeroVector;
	FVector ClientVelocity = FVector::ZeroVector;

	UPrimitiveComponent* GetRoot();
	FVector GetDeltaX(TArray< APlayerCharacter* >& PlayerList, APlayerCharacter*& out_pickupPlayer);

	void TryPickupStorable(APlayerCharacter* player);
	void TriggerPullInActor();
	
	bool FinishedWaiting() { return m_waitingTime >= EnablePullDelay; }
	
	bool SetState(walkPickup::State state);

	FString StateString(walkPickup::State state);
	walkPickup::State m_state;

	float m_waitingTime;
	float m_moveTime;

	bool m_pickedUp = false;
	float m_t;

	UFUNCTION()
	void ReplicateState(bool old);

	UPROPERTY(Replicated, ReplicatedUsing = ReplicateState)
	bool m_overlapping;
	
	TWeakObjectPtr< AStorableItem > mParentStorable;

};
