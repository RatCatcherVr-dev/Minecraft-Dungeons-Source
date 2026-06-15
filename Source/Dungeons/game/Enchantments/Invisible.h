#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Invisible.generated.h"

UCLASS()
class DUNGEONS_API UInvisibilityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UInvisibilityGameplayEffect();
};

UCLASS()
class DUNGEONS_API UInvisibilityFadeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UInvisibilityFadeGameplayEffect();

	static const FName DurationKey;
};


/**
 * 
 */
UCLASS()
class DUNGEONS_API UInvisible : public UEnchantment
{
	GENERATED_BODY()
public:
	UInvisible();

	void BeginPlay() override;
	void OnInvisibilityRemoved() override;

	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream&, FSharedPredictionContext) override;
	//virtual void OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey key) {}
	void OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey) override;
	void OnBeforeAoeAttack(AActor* attackTarget, FRandomStream&, FSharedPredictionContext) override;
	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData&, FRandomStream&) override;
	void OnEnd() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float FadeDurationSeconds = 2.f;

private:
	void RemoveAllEffects() const;
	void OnStunChanged(FGameplayTag, int32 tagCount);
};


namespace invisible {

bool isAllowedMob(const ABaseCharacter*);

}
