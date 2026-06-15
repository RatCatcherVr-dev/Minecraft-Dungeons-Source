#pragma once

#include "GameplayEffect.h"
#include "game/util/ActorQuery.h"
#include "game/component/AreaBuffComponent.h"
#include "game/actor/PropActor.h"
#include "ThrowablePropActor.h"
#include "ConduitItem.generated.h"

class ItemType;
class ABasePlayerController;
class APlayerCharacter;
class ABaseCharacter;

UCLASS()
class DUNGEONS_API AConduitItem : public AThrowablePropActor {
	GENERATED_BODY()
public:
	AConduitItem(const class FObjectInitializer& OI);

	/** Called when the actor falls out of the world 'safely' (below KillZ and such) */
	void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** Called when the Actor is outside the hard limit on world bounds */
	void OutsideWorldBounds() override;

	void ThrowInMouseDir() override;

	void Activate(const FVector& dir) override;

	void ThrowServer_Implementation(const FVector& dir) override;

	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void GiveConduitToCharacter(ACharacter* Character);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Default")
	void EnableSound(bool enable);

	UFUNCTION(BlueprintImplementableEvent, Category = "Default")
	void UpdateMaterials();

	UPROPERTY(ReplicatedUsing = OnRep_IsHeldByPlayer)
	bool IsHeldByPlayer;

	UFUNCTION()
	void OnRep_IsHeldByPlayer();

protected:
	void Tick(float DeltaSeconds) override;

	void OnLandedInternal();
	bool IsInUnsafeLocation() const;
	void TeleportToSafeLocation();
	ABaseCharacter* GetTeleportTarget() const;
	UBoxComponent* GetCollisionComponent() const;

	void LaunchOverlapDone(const FTraceHandle& TraceHandle, FOverlapDatum & TraceData);
	
	FOverlapDelegate LaunchOverlapDelegate;
	FVector LaunchImpulse;

};