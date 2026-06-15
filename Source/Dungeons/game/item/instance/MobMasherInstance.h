// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/abilities/prediction/GameplayPredictionExtensions.h"
#include "game/util/Pushback.h"

#include "MobMasherInstance.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UMobMashGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMobMashGameplayEffect();
};

UCLASS()
class DUNGEONS_API UMobMashShockwaveDamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UMobMashShockwaveDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API AMobMashShockWave : public AActor {
	GENERATED_BODY()
public:
	AMobMashShockWave();

	void BeginPlay() override;

	void Tick(float deltaTime) override;

	void SetDamage(float damage);
protected:	
	UPROPERTY(EditDefaultsOnly)
	float ExpandDurationSeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly)
	float ExpandTargetRadius = 500.f;

	UPROPERTY(EditDefaultsOnly)
	float ExpandRadiusExponent = 0.4f;

	UPROPERTY(EditDefaultsOnly)
	float LifeTime = 2.f;

	UPROPERTY(EditDefaultsOnly)
	FPushback HitPushback;
private:
	float mDamage = 1.f;

	USphereComponent* Sphere;

	float mExpandedTime = 0.0f;

	TArray< ABaseCharacter*> mHitTargets;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

USTRUCT()
struct DUNGEONS_API FMobToMash {
	GENERATED_USTRUCT_BODY()
public:
	FMobToMash(const ABaseCharacter* mob, bool primary);
	FMobToMash() {}
	UPROPERTY()
	const ABaseCharacter* Mob;
	bool Primary = false;
};

UCLASS()
class DUNGEONS_API AMobMasherInstance : public AItemInstance {
	GENERATED_BODY()
public:
	AMobMasherInstance();

	float GetStats(EItemStats stat) const override;

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason);

	void Activate(const FPredictionKey& predictionKey) override;
	bool CanActivate() const override;
	bool IsBusy() const override;
protected:
	void OnMobMasherStatusChanged(const FGameplayTag tag, const int32 tagCount);
	void OnMobMasherGainedLocally(const FGameplayTag tag, const int32 tagCount);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMobMashShockWave> MobMashShockWavePrimaryClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMobMashShockWave> MobMashShockWaveSecondaryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMobMashGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly)
	float SpawnDelayInitial = 0.25f;
	
	UPROPERTY(EditDefaultsOnly)
	float SpawnDelayPerTarget = 0.25f;

	UPROPERTY(EditDefaultsOnly)
	float SpawnDelayNumTargetsSpeedUpFactor = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	float ShockwaveDamage = 60.0f;
	
	// -1 for unlimited
	UPROPERTY(EditDefaultsOnly)
	int MaxTimesPerAttack = -1;

	// -1 for unlimited
	UPROPERTY(EditDefaultsOnly)
	int MaxTimesPerMob = -1;
	
	UPROPERTY(EditDefaultsOnly)
	int AttackStacks = 4;
private:
	
	void SpawnShockWave(FMobToMash mob);

	void SpawnShockwaveDelayed(const FMobToMash& mob, float delay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnShockwave(const FMobToMash& mob, float delay, FPredictionKey key);

	void OnMeleeDamageDealt(bool successfulAttack, const ABaseCharacter* fromCharacter, const ABaseCharacter* toCharacter, const FSharedPredictionContext& context);
	void OnAfterMeleeDamageDealt(bool successfulAttack, const ABaseCharacter* fromCharacter, const FSharedPredictionContext& context);	

	void ExecuteMobMash(const FSharedPredictionContext& context);

	bool IsMaxCountReached() const;
	bool IsMobMaxStrikeCountReached(const ABaseCharacter* mob) const;

	FActiveGameplayEffectHandle ActiveStatusEffectHandle;

	UPROPERTY()
	TArray<FMobToMash> MobsToMash;

	UPROPERTY()
	TArray<const ABaseCharacter*> AttackedMobsPerMash;

	FDelegateHandle delegateHandle;

	bool bLocallyBusy = false;
	
	static const FName StatusEffectGameplayTagName;
};
