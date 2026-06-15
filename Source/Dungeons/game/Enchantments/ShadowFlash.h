#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "ShadowFlash.generated.h"

UCLASS()
class DUNGEONS_API UShadowFlashGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UShadowFlashGameplayEffect();
};

// Damages nearby enemy mobs when exiting shadow form. Damage is based on weapon damage and enchantment level
UCLASS()
class DUNGEONS_API UShadowFlash : public UEnchantment
{
	GENERATED_BODY()

public:
	UShadowFlash();

private:

	enum ShadowFlashLevel
	{
		Level0,
		Level1,
		Level2,
		Level3,

		Count

	};

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = "0.0", UIMin = "0.0"))
	float Damage = 40.f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = "0", UIMin = "0"))
	int DamageRadiusPerLevel[ShadowFlashLevel::Count] = { 0, 2, 3, 4 };

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = "0.0", UIMin = "0.0"))
	float PushbackStrengthPerLevel[ShadowFlashLevel::Count] = { 0, 3.f, 4.f, 5.f };

	void OnInvisibilityTagCountChanged(const FGameplayTag tag, const int32 tagCount);

protected:
	void BeginPlay() override;
	void PostInitProperties() override;
};