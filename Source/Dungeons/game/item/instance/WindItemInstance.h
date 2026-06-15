#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/util/Pushback.h"
#include "WindItemInstance.generated.h"

UCLASS()
class DUNGEONS_API UWindHornSlowGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWindHornSlowGameplayEffect();

	static const FName DurationName;
	static const FName SpeedDividerName;
	static const FName MeleeSpeedDividerName;
	static const FName RangedSpeedDividerName;
};

UCLASS()
class DUNGEONS_API AWindItemInstance : public AItemInstance
{
	GENERATED_BODY()

	AWindItemInstance();
	
	int GetDisplayCount() const override;
	void ApplyWindhornEffectToMobs(APlayerCharacter* player,
	                               const FPredictionKey& predictionKey) const;

	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	FPushback WindPush;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	int32 PushRange = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float SlowDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float SlowMultiplier = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	TSubclassOf<UWindHornSlowGameplayEffect> Effect;

private:
	void ExecuteWindhornGameplayCue(const APlayerCharacter* player) const;
	TArray<class AMobCharacter*> GetMobsToTarget(const APlayerCharacter* player) const;

	FGameplayEffectSpec CreateSlowEffectSpec(UAbilitySystemComponent* abilitySystem) const;
};
