#pragma once
#include "CoreMinimal.h"
#include "Enchantment.h"
#include "GameplayEffect.h"
#include "Huge.generated.h"

UCLASS()
class DUNGEONS_API UHugeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UHugeGameplayEffect();
private:
	const float MaxHPMultiplier = 0.6f;
	const float AddedAbsoluteHP = 500.f;
};

UCLASS()
class DUNGEONS_API UHuge : public UEnchantment {
	GENERATED_BODY()
public:
	UHuge();

	void OnStart() override;
	void OnEnd() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float Scale = 3.f;
private:
	class UGrowComponent* GrowComponent;
	float CachedStartScale;
};

