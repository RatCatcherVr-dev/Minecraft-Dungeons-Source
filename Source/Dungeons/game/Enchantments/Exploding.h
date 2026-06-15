#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MeleeAttackComponent.h"
#include "Exploding.generated.h"

UCLASS()
class DUNGEONS_API UExplodingDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UExplodingDamageGameplayEffect();
};


UCLASS()
class DUNGEONS_API UExploding : public UEnchantment
{
	GENERATED_BODY()
	
public:
	UExploding();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UParticleSystem *ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class USoundCue *ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float explosionRadius = 450.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float explosionMaxHealthFactorBase = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float explosionMaxHealthFactorPerLevel = 0.2f;
	
	void OnExplodeCharacter(TWeakObjectPtr<ABaseCharacter> target);
	void SpawnExplosion(ABaseCharacter* target);
};



