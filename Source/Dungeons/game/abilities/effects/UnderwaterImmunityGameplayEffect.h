#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "game/Enchantments/Enchantment.h"
#include "UnderwaterImmunityGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UUnderwaterImmunityGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UUnderwaterImmunityGameplayEffect();
};

UCLASS()
class DUNGEONS_API UUnderwaterImmunity : public UEnchantment
{
	GENERATED_BODY()
public:
	UUnderwaterImmunity();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	FActiveGameplayEffectHandle Handle;
};
