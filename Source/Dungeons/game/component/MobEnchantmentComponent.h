#pragma once

#include "CoreMinimal.h"
#include "game/component/EnchantmentComponent.h"
#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "MobEnchantmentComponent.generated.h"

UCLASS(Within = MobCharacter)
class DUNGEONS_API UMobEnchantmentComponent : public UEnchantmentComponent
{
	GENERATED_BODY()

	void OnRep_Enchantments() override;

	void BeginPlay() override;

public:
	bool IsEnchanted() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FEnchantmentData> GetEnchantmentsNonInherent() const;

	UPROPERTY(EditDefaultsOnly)
	float AncientScaleMultiplier = 1.5f;

private:
	void OnEnchantmentRegistered(UEnchantment*) override;
	
	void OnEnchanted(const FGameplayTag tag, int32 tagCount);

	AMobCharacter* GetOwnerMobCharacter() const;

	FActiveGameplayEffectHandle handle;
};

UCLASS()
class DUNGEONS_API UEnchantedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UEnchantedGameplayEffect();
};


UCLASS()
class DUNGEONS_API UAncientEnchantedGameplayEffect : public UEnchantedGameplayEffect {
	GENERATED_BODY()
public:
	UAncientEnchantedGameplayEffect();

	static constexpr float MovementSpeedMultiplier = 1.5f;
	static constexpr float DamageMultiplier = 1.75f;

	static constexpr float BonusMaxHealthFraction = 0.25f;
	static constexpr float BonusMaxHealthAbsolute = 2000.f;
};

UCLASS() 
class DUNGEONS_API UEnchantedHpBoostGameplayEffect : public UEnchantedGameplayEffect {
	GENERATED_BODY()
public:
	const float BonusMaxHealthFraction = 0.55f;
	const float BonusMaxHealthAbsolute = 450.f;

	UEnchantedHpBoostGameplayEffect();
};
