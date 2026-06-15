#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "WildRage.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API UWildRageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWildRageGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UWildRage : public UEnchantment
{
	GENERATED_BODY()
public:
	UWildRage();

protected:
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWildRageGameplayEffect> EffectClass;
private:
	bool CanRage(AMobCharacter*) const;

	void OnRage(const FGameplayTag tag, const int32 tagCount, TWeakObjectPtr<AMobCharacter> mob);
	
	UFUNCTION()
	void HandleMobKillMob(AActor* effectCauser, AActor* mob, AActor* mobKilled);
};
