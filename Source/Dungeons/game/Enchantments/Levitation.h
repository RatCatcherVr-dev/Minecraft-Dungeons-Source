#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>

#include "Levitation.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API ULevitationGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	ULevitationGameplayEffect();
	static const FName EffectMagnitudeName;
};

UCLASS()
class DUNGEONS_API ULevitation : public UEnchantment
{
	GENERATED_BODY()
public:
	ULevitation();

protected:
	void AfterDealtDamage(AActor* toWhat);

	void OnDodgeRollEnd(FPredictionKey) override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	bool CanActivate = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
		float Duration = 2.f;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", DisplayName = "Extra Duration Per Level")
		float DurationPerLevel = 1.f;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", DisplayName = "Accelleration")
	float EffectStrength = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float LaunchStrength = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float FallDamagePercentagePerLevel = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", DisplayName = "Levitation Ready Effect")
	TSubclassOf<ULevitationReadyGameplayEffect> Effect;
	FActiveGameplayEffectHandle LevitationReadyEffectHandle;
};

UCLASS()
class DUNGEONS_API ULevitationMelee : public ULevitation
{
	GENERATED_BODY()
public:
	ULevitationMelee();

public:
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
};

UCLASS()
class DUNGEONS_API ULevitationRanged : public ULevitation
{
	GENERATED_BODY()
		ULevitationRanged();

public:
	void OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};

UCLASS()
class DUNGEONS_API ULevitationImmunityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	ULevitationImmunityGameplayEffect();
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API ULevitationFallGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	ULevitationFallGameplayEffect();
	static const FName FallMultiplierName;
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API ULevitationReadyGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	ULevitationReadyGameplayEffect();
};