#pragma once

#include "AItemInstance.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "TargetingStrikeItem.h"
#include "game/Enchantments/FireAspect.h"
#include "SatchelOfTheElementsInstance.generated.h"

class AMobCharacter;

enum class ESatchelEffect {
	Burning,
	Freezing,
	Shocking,

	MAX
};

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API ASatchelOfTheElementsInstance : public AItemInstance {
	GENERATED_BODY()
public:
	ASatchelOfTheElementsInstance();
	void Activate(const FPredictionKey& predictionKey) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int EffectRange = 1200;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int MaxMobAttackTotal = 7;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Freeze")
	float FreezeSpeedPercentage = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Lightning")
	float LightningDamage = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Lightning")
	float LightningChanceWeight = 0.45f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Fire")
	float FireDamage = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Fire")
	float FirePeriod = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Fire")
	float FireChanceWeight = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float EffectDuration = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float FreezeEffectDuration = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<float> TargetCountCooldownPercentage;


protected:
	void PopulateBulletPoints(TArray<FItemBulletPoint>&, const ABaseCharacter& owner) const override;

public:
	int GetDisplayCount() const override { return 0; };
	float GetStats(EItemStats stat) const override;

private:
	void LightningStrike(AMobCharacter& target);
	void Burn(AMobCharacter& target);
	void Freeze(AMobCharacter& target);

	void AddPushbackToSpec(FGameplayEffectSpec&, const AActor&);

	ESatchelEffect GetRandomEffect();

	float CalculateCooldown(int targetCount);
};

UCLASS()
class DUNGEONS_API USatchelOfTheElementsLightningStrikeGameplayEffect : public UTargetingStrikeDamageItemGameplayEffect {
	GENERATED_BODY()
public:
	USatchelOfTheElementsLightningStrikeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API USatchelOfTheElementsBurningGameplayEffect : public UFireAspectGameplayEffect {
	GENERATED_BODY()
public:
	USatchelOfTheElementsBurningGameplayEffect();
};