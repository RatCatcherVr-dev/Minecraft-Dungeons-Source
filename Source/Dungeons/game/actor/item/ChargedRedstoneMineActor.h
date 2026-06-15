#pragma once

#include "GameplayEffect.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/util/ActorQuery.h"
#include "game/util/Pushback.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/actor/PropActor.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "ChargedRedstoneMineActor.generated.h"

class ItemType;
class ABasePlayerController;
class APlayerCharacter;
class AChargedRedstoneMinesInstance;

UCLASS()
class DUNGEONS_API AChargedRedstoneMineActor : public APropActor_RepSpatializeStatic {
	GENERATED_BODY()
public:
	AChargedRedstoneMineActor(const class FObjectInitializer& OI);

	UFUNCTION(BlueprintCallable)
	void Explode();

	UFUNCTION(BlueprintImplementableEvent)
	void OnExplode();

	void Init(const FGameplayEffectSpec& hostile, const FGameplayEffectSpec& friendly);

	void BeginPlay() override;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void LifeSpanExpired();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastExplode();

	UPROPERTY()
	FGameplayEffectSpec HostileSpec;

	UPROPERTY()
	FGameplayEffectSpec FriendlySpec;

	UPROPERTY(EditDefaultsOnly)
	float ExplosionRadius = 250.f;

	UPROPERTY(EditDefaultsOnly)
	float MineKnockbackStrength = 6.f;

	UPROPERTY(EditDefaultsOnly)
	FPushback Pushback;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* MineCollision;
private:
	bool bShouldExpire = false;
};
