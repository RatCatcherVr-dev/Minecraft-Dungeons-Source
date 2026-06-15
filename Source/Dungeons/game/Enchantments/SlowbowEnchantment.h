#pragma once

#include "CoreMinimal.h"
#include "Enchantment.h"
#include "Freezing.h"
#include "SlowbowEnchantment.generated.h"

UCLASS()
class DUNGEONS_API USlowBowEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	USlowBowEnchantment();

	void OnProjectileLaunch(ABaseProjectile* fromProjectile) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float mDuration = 3.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float mInitialFreezeAmount = 0.5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float mFreezePerLevel = 0.1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UFreezingGameplayEffect> mGameplayEffect;

};
