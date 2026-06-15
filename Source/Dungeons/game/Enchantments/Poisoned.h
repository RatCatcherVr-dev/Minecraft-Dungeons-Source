#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/actor/EffectGroup/EffectGroup.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/item/BaseProjectile.h"
#include "Poisoned.generated.h"

UCLASS()
class DUNGEONS_API UPoisonCloudSpawnHelper : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static APoisonCloud* SpawnPoisonCloud(TSubclassOf<class APoisonCloud> poisonCloudClass, TSubclassOf<class UPoisonCloudDamageGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, float level, float damagePerSecond, float duration);
private:
	static FGameplayEffectSpec CreatePoisonCloudDamageEffectSpec(TSubclassOf<UPoisonCloudDamageGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, float level, float damagePerSecond, APoisonCloud* poisonCloud, float duration);
};

UCLASS()
class DUNGEONS_API APoisonCloud : public ABaseProjectileProp
{
	GENERATED_BODY()
public:
	APoisonCloud();

	void Tick(float DeltaSecs) override;

	void BeginPlay() override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void Reset() override;

	void SetGameplaySpec(FGameplayEffectSpec&& spec);

	void StartCloud(float lifeTime);

	class ABaseCharacter* GetCharacterOwner() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TSubclassOf<ABaseCharacter>> characterClassesToExclude;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AreaOfEffect = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamageDisableTimeBeforeDestroy = 0.5f;

	UFUNCTION(BlueprintNativeEvent)
	void OnDamageDisabled();

	UFUNCTION(BlueprintNativeEvent)
	void OnCloudStarted();
	virtual void OnCloudStarted_Implementation(){};

private:
	void RemoveEffectOnCharacter(ABaseCharacter*);

	bool bDealingDamage = true;

	FTimerHandle DamageDisableTimerHandle;

	USphereComponent* Sphere;
	FGameplayEffectSpec Spec;

	TArray< TWeakObjectPtr< ABaseCharacter > > PoisonList;
};

UCLASS()
class DUNGEONS_API UPoisoned : public UEnchantment
{
	GENERATED_BODY()

	void BeginPlay() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<APoisonCloud> PosionCloudClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MobDamagePerSecond = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BaseDamagePerSecond = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TriggerChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CloudDuration = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MobCloudDuration = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<TSubclassOf<ABaseCharacter>, TSubclassOf<AReusedEffectActor>> EffectMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffectGroupParams EffectGroupParameters;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnCloudEffect(const FTransform& transform);

	TSubclassOf<AReusedEffectActor> GetEffectClass();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnStart() override;

	UFUNCTION()
	void PreCachePoisonClass();
};

UCLASS()
class DUNGEONS_API UPoisonedMelee : public UPoisoned
{
	GENERATED_BODY()

	UPoisonedMelee();
	
	FText CreateDescription() const override;

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
};


UCLASS()
class DUNGEONS_API UPoisonedRanged : public UPoisoned
{
	GENERATED_BODY()

	UPoisonedRanged();

	FText CreateDescription() const override;


	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};

UCLASS(Abstract)
class DUNGEONS_API UPoisonCloudDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPoisonCloudDamageGameplayEffect();
protected:
	void AddHealthModifier(UClass* modDamageCalculationClass);
};

UCLASS()
class DUNGEONS_API UPoisonRangedCloudDamageGameplayEffect : public UPoisonCloudDamageGameplayEffect {
	GENERATED_BODY()
public:	
	UPoisonRangedCloudDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UPoisonMeleeCloudDamageGameplayEffect : public UPoisonCloudDamageGameplayEffect {
	GENERATED_BODY()
public:
	UPoisonMeleeCloudDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UPoisonItemCloudDamageGameplayEffect : public UPoisonCloudDamageGameplayEffect {
	GENERATED_BODY()
public:
	UPoisonItemCloudDamageGameplayEffect();
};