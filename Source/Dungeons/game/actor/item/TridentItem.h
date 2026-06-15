#pragma once

#include "GameplayEffect.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/util/ActorQuery.h"
#include "game/util/Pushback.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/actor/PropActor.h"
#include "ThrowablePropActor.h"
#include "game/actor/item/Arrow.h"
#include "game/actor/ImpactActionHandler.h"
#include "TridentItem.generated.h"

class ItemType;
class ABasePlayerController;
class APlayerCharacter;

UCLASS()
class DUNGEONS_API ATridentItem : public AThrowablePropActor {
	GENERATED_BODY()
public:
	ATridentItem(const class FObjectInitializer& OI);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UStaticMeshComponent* TridentMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Dungeons")
	class UAudioComponent* MainSoundComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	class USoundCue* ThrownSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FName EmissiveMaterialName = "EmissiveBPower";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABaseProjectile> TridentProjectile = nullptr;
	

	void BeginPlay() override;

	void ThrowInMouseDir() override;

	void ThrowInPlayerForwardDir() override;

	void Activate(const FVector& dir) override;

	void ThrowServer_Implementation(const FVector& dir) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnProjectileServer(ABaseCharacter* targetMob, FRotator newArrowRotation);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnProjectileMulticast(ABaseCharacter* targetMob, FRotator newArrowRotation);


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Trident Variables")
	int32 HitDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Trident Variables")
	float MobPushbackStrength = 1.0f;
	
	FGameplayEffectSpec GetGameplayEffectSpec(UAbilitySystemComponent* AbilitySystemComponent, AActor* owner, const float scaledDamage) override;
	
};


UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UTridentExplosionAction : public UDelayedExplosionAction {
	GENERATED_BODY()
public:
	UTridentExplosionAction();
};

UCLASS()
class DUNGEONS_API UTridentDamageGameplayEffect : public UWorldDamageGameplayEffect {
	GENERATED_BODY()
public:
	UTridentDamageGameplayEffect();

};

UCLASS()
class DUNGEONS_API ATridentItemArrowProp : public AArrowProp {
	GENERATED_BODY()


public:
	ATridentItemArrowProp(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void ExplodeNative(float radius, FGameplayEffectSpec spec, TWeakObjectPtr<UAbilitySystemComponent> instigatorAbilitySystem, const FPushback& pushback, FGameplayTag cue);
	
	virtual void InitialiseProp(bool bCharged, float fPropLifetime) override;

	void EmissiveChange();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FName EmissiveMaterialName = "EmissiveBPower";

	UPROPERTY(Transient)
	FTimerHandle EmissiveTimerHandle;

	float mCurrentEmissiveValue = 0.0f;
	float mEffectTime = 0.0f;

};
