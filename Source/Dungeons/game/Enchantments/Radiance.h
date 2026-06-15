#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Radiance.generated.h"

UCLASS()
class DUNGEONS_API URadianceGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URadianceGameplayEffect();
	void AddHealthModifier(UClass* modDamageCalculationClass);
};

UCLASS()
class DUNGEONS_API URadianceRangedGameplayEffect : public URadianceGameplayEffect {
	GENERATED_BODY()
public:
	URadianceRangedGameplayEffect();
};

UCLASS()
class DUNGEONS_API URadianceMeleeGameplayEffect : public URadianceGameplayEffect {
	GENERATED_BODY()
public:
	URadianceMeleeGameplayEffect();
};

UCLASS()
class DUNGEONS_API AHealArea : public AActor
{
	GENERATED_BODY()
public:
	AHealArea();

	void SetDuration(float durationSeconds);
	void SetExpandSizePerSecond(float expandSize);
	void SetHealAmount(float amount);
	void SetGameplayEffect(TSubclassOf<class URadianceGameplayEffect> gameplayEffect);
protected:
	void BeginPlay() override;

	void Tick(float DeltaTime) override;
private:
	TSubclassOf<class URadianceGameplayEffect> Effect = URadianceGameplayEffect::StaticClass();
	float Duration = 0.5f;
	float ExpandSizePerSecond = 1500.0f;

	class ABaseCharacter* GetCharacterOwner() const;

	FTimerHandle ExpandTimerhandle;

	float HealCircleRadius = 0.0f;	
	float HealAmount = 50.0f;

	TArray<TWeakObjectPtr<class AActor>> HealedAllies;
};

UCLASS()
class DUNGEONS_API URadiance : public UEnchantment
{
	GENERATED_BODY()
public:
	URadiance();
	
	FText CreateDescription() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AHealArea> HealAreaClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealAreaExpandSizePerSecond = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealAreaDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealChance = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseHealing = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobHealing = 350.f;

	void SpawnRadiance(FVector location, float HealingAmount, TSubclassOf<class URadianceGameplayEffect> gameplayEffect) const;
};

UCLASS()
class DUNGEONS_API URadianceMelee : public URadiance
{
	GENERATED_BODY()
public:
	URadianceMelee();

	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext context) override;

	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
private:
	bool bCanTriggerRadiance = false;
	bool bHasTriggedCue = false;
};

UCLASS()
class DUNGEONS_API URadianceRanged : public URadiance
{
	GENERATED_BODY()
public:
	URadianceRanged();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};
