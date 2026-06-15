#pragma once
#include "Dungeons.h"
#include "CoreMinimal.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "game/component/MoveToTargetMovementComponent.h"
#include "game/item/instance/AItemInstance.h"
#include "game/util/SimpleMovementComponent.h"
#include "character/DungeonsAbilitySystemComponent.h"

#include "ShulkerBullet.generated.h"

UCLASS()
class DUNGEONS_API AShulkerBullet : public AActor_RepSpatializeDynamic {
	GENERATED_BODY()
public:
	AShulkerBullet();
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Tick(float DeltaSeconds) override;
	void OnSummonPlayed();
	void SetTarget(AActor* target);
	void Cleanup();
	AActor* GetNearestTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
		UBoxComponent* mBoxCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
		USimpleMovementComponent* mMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
		UMoveToTargetMovementComponent* mTrackingComponent;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		class UDungeonsAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Damage")
		float Damage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Levitation")
		float levitationDuration = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Levitation")
		float fallDuration = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Levitation", DisplayName = "Accelleration")
		float LevitationStrength = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Levitation")
		float LevitationDropDamagePercentage = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Levitation")
		float LaunchStrength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Target")
		float targetPlayerDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shulker Bullet | Target")
		float targetPowerStations = 500.0f;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		virtual void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:

	UPROPERTY(Replicated)
		AActor* mTrackingTarget;

	UPROPERTY()
		FTimerHandle mTimeoutHandle;

	UPROPERTY()
		float mTimeout = 15.f;
};