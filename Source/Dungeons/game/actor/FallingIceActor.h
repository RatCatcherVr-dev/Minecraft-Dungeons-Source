#pragma once
#include "Dungeons.h"
#include "CoreMinimal.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "GameplayEffect.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/item/instance/AItemInstance.h"
#include "game/util/SimpleMovementComponent.h"
#include "UnrealNetwork.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"

#include "FallingIceActor.generated.h"

// ----- Falling Ice Gameplay Effect ----- //
// Applied upon "collision" with a falling ice cube actor
UCLASS()
class DUNGEONS_API UFallingIceDamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UFallingIceDamageGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

// ----- Falling Ice Actor ----- //
UCLASS()
class DUNGEONS_API AFallingIceActor : public AActor_RepSpatializeDynamic {
	GENERATED_BODY()
public:
	AFallingIceActor();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual void OnSummonPlayed();

	// Called when summoned by IceWand
	UFUNCTION(BlueprintCallable)
	void SetScaledExplosionDamage(float damage) { mExplosionDamage = damage; };

	UFUNCTION(BlueprintImplementableEvent)
	void OnExplode();

	UFUNCTION()
	void DropIceblock();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mDropDelay = 0.25f;

	// This is an applied downward velocity.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mDropSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mExplosionDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mExplosionInnerRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mExplosionOuterRange = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mExplosionDamageRangeFalloff = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bOverwriteStunDuration = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", meta = (EditCondition = "bOverwriteStunDuration", ClampMin = "0.0"))
	float mStunDuration = 2.f;

protected:
	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnIceblockCollision();

	void CreateExplosionEffect(const ABaseCharacter* ownerCharacter);

	void IceblockCleanup();

	UPROPERTY()
	float mDropTimeout = 10.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStaggerGameplayEffect> StunEffect = UStaggerGameplayEffect::StaticClass();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	UBoxComponent* mBoxCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	USimpleMovementComponent* mMovementComponent;

	UPROPERTY()
	FTimerHandle mDropTimerHandle;

	UPROPERTY()
	FTimerHandle mDropTimeoutHandle;
};