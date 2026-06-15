#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/component/SoulComponent.h"
#include "ShadowFeast.generated.h"

UCLASS()
class DUNGEONS_API UShadowFeastGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UShadowFeastGameplayEffect();
};

class AActor;

UCLASS()
class DUNGEONS_API UShadowFeast : public UEnchantment
{
	GENERATED_BODY()

public:
	UShadowFeast();

protected:
	void BeginPlay() override;

private:

	enum ShadowFeastLevel
	{
		Level0,
		Level1,
		Level2,
		Level3,

		Count
	};

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int soulSpawnsPerLevel[ShadowFeastLevel::Count] = { 0,2,4,6 };

	UFUNCTION()
	void OnAnyMobKilled(AActor* killedBy);

	FGameplayTag GetInvisibleTag();
};
