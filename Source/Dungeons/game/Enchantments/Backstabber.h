#pragma once
#include "game/Enchantments/Enchantment.h"
#include "CoreMinimal.h"

#include "Backstabber.generated.h"


UCLASS()
class DUNGEONS_API UBackstabber : public UEnchantment
{
	GENERATED_BODY()
public:
	UBackstabber();

	void PostInitProperties() override;
	FText CreateDescription() const override;

	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) override;

	UPROPERTY(EditDefaultsOnly)
		float DamageMultiplier = 0.15f;

};