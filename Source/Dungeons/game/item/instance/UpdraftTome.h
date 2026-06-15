#pragma once

#include "Dungeons.h"
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"

#include "UpdraftTome.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API UUpdraftTomeGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UUpdraftTomeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UUpdraftTomeTargetGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UUpdraftTomeTargetGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UUpdraftTomeImmunityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UUpdraftTomeImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API AUpdraftTomeInstance : public AItemInstance {
	GENERATED_BODY()
public:
	AUpdraftTomeInstance();

	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const override;

protected:

	void PopulateBulletPoints(TArray<FItemBulletPoint>&, const ABaseCharacter&) const override;

	bool IsBusy() const override;

	UFUNCTION()
	void ApplyUpdraft(const FPredictionKey predictionKey);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float UpdraftLaunchHeight = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float UpdraftDamage = 520.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float UpdraftStunDuration = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float UpdraftRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int UpdraftMaxTargets = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float UpdraftEffectDelay = 1.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float UpdraftApplyImmunityDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UUpdraftTomeGameplayEffect> UpdraftTomeEffect = nullptr;
	
	TArray<TWeakObjectPtr<ABaseCharacter>> AttackTargets;

private:
	FTimerHandle AttackDelayHandle;
};