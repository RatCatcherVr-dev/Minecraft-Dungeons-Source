#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/item/ItemSlot.h"
#include "game/abilities/effects/DamageWeakeningGameplayEffect.h"
#include "Weakening.generated.h"


UCLASS()
class DUNGEONS_API UWeakening : public UEnchantment {
	GENERATED_BODY()
	
public:
	UWeakening();

	FText CreateDescription() const override;
	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);
	void OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float WeakenRange = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TArray<float> WeakenAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float MobWeakenAmount = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float MobDuration = 5.f;

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WeakeningDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDamageWeakeningGameplayEffect> Effect;

	FGameplayEffectSpec CreateWeakenEffectSpec(UAbilitySystemComponent* abilitySystem) const;
	TArray<ABaseCharacter*> GetNearbyTargets(ABaseCharacter* characterTarget, const ABaseCharacter* characterOwner);

	bool ApplyDebuff(ABaseCharacter* target, FSharedPredictionContext context);
};


